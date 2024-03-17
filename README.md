# ft_irc

## 기본 연결 흐름

1. 클라이언트는 IRC 서버와 6667(기본) 또는 지정된 포트로 TCP 연결을 설정합니다.
2. 클라이언트는 `NICK` 명령을 보내 닉네임을 설정합니다.
3. 클라이언트는 `USER` 명령을 보내 사용자 정보 (사용자 이름, 호스트 이름, 서버 이름, 실제 이름)를 제공합니다.
4. 서버는 연결의 성공 또는 실패를 나타내는 일련의 숫자 코드로 응답합니다.
5. 연결이 설정되면 클라이언트는 `JOIN` 명령을 사용하여 채널에 참여할 수 있습니다.
6. 서버는 `MOTD` 를 클라이언트에게 보내 서버에 대한 정보를 제공합니다.
7. 클라이언트는 여러 명령을 사용하여 채널에서 대화를 나눌 수 있습니다.

## 명령어

일단 레퍼런스 긁어온거라 얼마나 구현할지는 결정해야함.

모든 명령어는 대소문자를 구분하지 않고 `/r/n`으로 끝납니다.

```
message		::= ['@' <tags> SPACE] [':' <source> SPACE] <command> <parameters> <crlf>
SPACE		::=  %x20 *( %x20 )   ; space character(s)
command		::=  letter* / 3digit
crlf		::=  %x0D %x0A        ; "carriage return" "linefeed"
parameter	::=  *( SPACE middle ) [ SPACE ":" trailing ]
nospcrlfcl	::=  <sequence of any characters except NUL, CR, LF, colon (`:`) and SPACE>
middle		::=  nospcrlfcl *( ":" / nospcrlfcl )
trailing	::=  *( ":" / " " / nospcrlfcl )
mask		=  *( nowild / noesc wildone / noesc wildmany )
wildone		=  %x3F
wildmany	=  %x2A
nowild		=  %x01-29 / %x2B-3E / %x40-FF ; any octet except NUL, "*", "?"
noesc		=  %x01-5B / %x5D-FF ; any octet except NUL and "\"
matchone	=  %x01-FF ; matches wildone
matchmany	=  *matchone ; matches wildmany
```

- 태그 : (`@`, 0x40) 로 시작하는 메시지의 선택적 메타데이터입니다.
- source : (`:`, 0x3A) 로 시작하는 메시지의 출처에 대한 선택적 메모입니다.
- command : 이 메시지가 나타내는 특정 명령입니다.
- 매개변수 : 존재하는 경우 이 특정 명령과 관련된 데이터입니다.

이러한 메시지 부분과 매개변수 자체는 하나 이상의 ASCII SPACE 문자로 구분됩니다 (` `, 0x20).

<aside>
💡 numeric reply: 서버가 클라이언트에게 커멘드나 operation의 결과에 관해 전달하는 standardized messages

</aside>

## (1) connection registration

- IRC 서버에 연결을 등록하기 위해 사용

### PASS: 서버에 연결하기 위한 비번 설정

- 구성
  - 비번은 connection을 등록하기 위한 어느 시도 이전, 반드시 설정되어야 한다
    - 즉, 클라이언트는 NICK/USER 커멘드 이전에 서버 비번을 적어야 하고, 서버는 어떤 SERVER 커멘드보다 먼저 PASS 커멘드를 보내야 한다
  - It is possible to send multiple PASS commands
    before registering but only the last one sent is used for
    verification and it may not be changed once registered.

```jsx
PASS 대충비번
```

- numeric replies
  - `ERR_NEEDMOREPARAMS` - Not enough parameters
  - `ERR_ALREADYREGISTRED` - Already registered
  - `ERR_PASSWDMISMATCH` - Password incorrect

### NICK: 클라이언트가 nickname 설정

- 구성
  - hopcount는 서버에 의해서만 사용, 그 별명이 홈 서버에서 얼마나 멀리 있는지 알려주기 위해 사용(사용할 일 없음)

```jsx
NICK 대충별명 [ <hopcount> ]

:WiZ NICK Kilroy                ; WiZ changed his nickname to Kilroy.
```

- numeric replies
  - `ERR_NONICKNAMEGIVEN` - No nickname given
  - `ERR_ERRONEUSNICKNAME` - Erroneous nickname
  - `ERR_NICKNAMEINUSE` - Nickname in use
  - `ERR_NICKCOLLISION` - Nickname collision KILL(동시에 같은 닉네임을 사용하는 사용자가 있을 때 발생)

### USER: 새로운 클라이언트의 username, hostname, servername, realname 설정

- 구성
  - USER와 NICK 커맨드가 서버에 전달되어야지 IRC 서버에 클라이언트가 등록된다
  - realname 파라미터는 space(’ ‘) character들을 포함할 수 있고 이를 위해(하나의 파라미터로 인식시키기 위해) 콜론(’:’)이 선행되어야 한다

```jsx
USER <username> <hostname> <servername> <realname>
```

- numeric replies
  - `ERR_NEEDMOREPARAMS` - Not enough parameters
  - `ERR_ALREADYREGISTRED` - Already registered

### OPER: 일반 유저가 OP의 권한을 얻음

- 구성
  - 만약 OPER 커멘드를 사용한 클라이언트가 올바른 비번을 사용했다면, 서버는 나머지 네트워크에 클라이언트들의 nickname을 위한 “MODE +o”를 발급함으로써 알린다

```jsx
OPER 유저 비번
```

- numeric replies
  - `ERR_NEEDMOREPARAMS` - Not enough parameters
  - `ERR_NOOPERHOST` - Not vaild for your host
  - `ERR_PASSWDMISMATCH` - Password incorrect
  - `RPL_YOUREOPER` - You are now an IRC operator

### QUIT: 클라이언트 세션을 종료(서버에서 나감)

- 구성
  - 종료 메시지를 입력하면, 기본 메시지인 클라이언트의 nickname이 아닌 메시지가 출력
  - 만약 다른 이유로 인해 클라이언트 연결이 클라이언트가 QUIT 명령을 내리지 않고 종료된 경우(예: 클라이언트가 죽고 소켓에서 EOF 발생), 서버는 종료 메시지를 발생시킨 이벤트의 성격을 반영하는 어떤 종류의 메시지로 채워야 한다
  - 콜론이 선행된다면, 이후의 종료 메시지는 공백(’ ‘)을 포함하는 것으로 간주해야 한

```jsx
 QUIT 종료 메시지(선택적)

 QUIT :Gone to have lunch
```

- numeric replies
  - 없음

## (2) channel operations

### JOIN: 클라이언트가 특정 채널에 참여

- 구성
  - 아래와 같은 조건이 충족되어야 채널에 참여할 수 있다
    - 채널이 invite-only라면 클라이언트는 반드시 초대받아야 한다
    - 클라이언트의 nickname, username, hostname이 active ban에 해당되지 않아야 한다
    - key(비번)이 설정되었다면, 올바른 key(비번)이 주어져야 한다
  - 클라이언트가 채널에 참여한 후에는, 그들은 채널에 영향을 주는 모든 커멘드에 대한 알림을 받는다
    - 이 것은 `MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE` 을 포함한다
  - 만약 JOIN 커멘드가 성공적으로 실행되었다면, 클라이언트는 그 다

```jsx
JOIN <channel>{,<channel>} [<key>{,<key>}]

JOIN #foobar
JOIN &foo fubar
JOIN #foo,#bar fubar,foobar

:Wiz JOIN #Twilight_zone ;JOIN message from WiZ
```

- numeric replies
  - `ERR_NEEDMOREPARAMS (461)`: not enough parameter
  - `ERR_BADCHANMASK (476`): 클라이언트가 잘못된 채널 마스크를 사용하여 채널에 참여하려고 시도할 때 발생
    - 채널 마스크는 채널 이름을 지정하는 데 사용되는 구문을 의미하며, 형식이 잘못되거나 유효하지 않은 경우
  - `ERR_INVITEONLYCHAN (473)`: 클라이언트가 참여하려는 채널이 초대 전용임을 나타내는 응답
    - 이 모드를 갖는 채널은 클라이언트가 초대를 받기 전에는 참여할 수 없습니다.
  - `ERR_BANNEDFROMCHAN (474)`: 클라이언트가 지정된 채널 참여를 금지당했음을 나타내는 응답
    - 클라이언트가 금지되면 채널에 참여할 수 없으며, 서버가 이 오류 메시지를 보냅니다.
  - `ERR_BADCHANNELKEY (475)`: 클라이언트가 키가 설정된 채널에 참여하려고 할 때 잘못된 또는 유효하지 않은 채널 키를 제공한 경우 발생하는 응답
  - `ERR_CHANNELISFULL (471)`: 클라이언트가 참여하려는 채널이 이미 최대 용량에 도달하여 추가 사용자를 수용할 수 없음을 나타내는 응답
  - `ERR_NOSUCHCHANNEL (403):` 지정된 채널이 서버에 존재하지 않음을 나타내는 응답
    - 클라이언트가 등록되지 않거나 사용할 수 없는 채널에 참여하려고 할 때 서버가 이 오류 메시지를 보냅니다.

### PART: 클라이언트의 채널 나가기

- 구성
  - 파라미터로 제시된 채널에서 active user 목록에서 제거

```jsx
PART <channel>{,<channel>}

PART #oz-ops,&groups
```

- numeric replies
  - `ERR_NEEDMOREPARAMS`
  - `ERR_NOSUCHCHANNEL`
  - `ERR_NOTONCHANNEL:` 사용자가 지정한 채널에 참여하지 않았을 때 발생하는 오류를 나타냅니다. 사용자가 채널에 참여하지 않은 상태에서 채널 관련 명령을 실행하려고 할 때 서버가 이 오류를 반환합니다.

### MODE: channel의 모드를 변경

- 구성

1. channel 모드

```jsx
MODE <channel> {[+|-]|o|i|k|o|l} [<limit>] [<user>] [<ban mask>]

MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on channel #Finnish.
```

- i: 초대 전용 채널 설정/해제
- t: 채널에서 주제 명령의 제한 설정/해제 (채널 운영자에게만 적용)
- k: 채널 키(비밀번호) 설정/해제
- o: 채널 운영자 권한 부여/박탈
- l: 채널의 사용자 제한 설정/해제

- numeric replies
  - `ERR_NEEDMOREPARAMS`: 명령에 필요한 매개변수가 충분하지 않음을 나타냅니다.
  - `RPL_CHANNELMODEIS`: 채널의 현재 모드를 보여줍니다.
  - `ERR_CHANOPRIVSNEEDED`: 채널 운영자 권한이 필요함을 나타냅니다.
  - `ERR_NOSUCHNICK`: 지정된 닉네임이 존재하지 않음을 나타냅니다.
  - `ERR_NOTONCHANNEL`: 클라이언트가 채널에 참여하지 않았음을 나타냅니다.
  - `ERR_KEYSET`: 채널 키가 이미 설정되었음을 나타냅니다.
  - `RPL_BANLIST`: 채널의 금지된 사용자 목록을 표시합니다.
  - `RPL_ENDOFBANLIST`: 금지된 사용자 목록의 끝임을 나타냅니다.
  - `ERR_UNKNOWNMODE`: 알 수 없는 모드 문자가 사용되었음을 나타냅니다.
  - `ERR_NOSUCHCHANNEL`: 지정된 채널이 존재하지 않음을 나타냅니다.
  - `ERR_USERSDONTMATCH`: 사용자가 서로 일치하지 않음을 나타냅니다.
  - `RPL_UMODEIS`: 사용자의 현재 모드를 표시합니다.
  - `ERR_UMODEUNKNOWNFLAG`: 알 수 없는 사용자 모드 플래그가 지정되었음을 나타냅니다.

### TOPIC: 채널의 topic 확인 또는 변경

- 구성
  - <topic>이 주어지지 않으면 채널 <channel>의 주제가 반환된다
  - <topic> 매개변수가 있는 경우, 해당 채널의 주제가 변경된다
    - 이때 채널 모드가 이 작업을 허용하는 경우에만 변경된다.

```jsx
TOPIC <channel> [<topic>]
```

- numerics
  - `ERR_NEEDMOREPARAMS`: 필요한 매개변수가 더 필요함
    `ERR_NOTONCHANNEL`: 채널에 참여하지 않음
    `RPL_NOTOPIC`: 주제가 설정되지 않음
    `RPL_TOPIC`: 주제가 설정됨
    `ERR_CHANOPRIVSNEEDED`: 채널 운영자 권한이 필요함

### NAMES: 클라이언트가 볼 수 있는 nickname 목록 나열

- 구성
  - 채널이 private이나 secret을 제외한 목록 확인 가능
  - 틀린 채널 이름에 대한 에러 메시지는 없다
  - 파라미터가 존재하지 않는다면, 모든 채널과 그에 속해있는 참여자들의 목록이 나열된다

```jsx
NAMES [<channel>{,<channel>}]

NAMES #twilight_zone,#42
NAMES
```

- numerics
  - `RPL_NAMREPLY`: 채널의 사용자 목록을 클라이언트에게 제공하는 숫자 응답 코드입니다. 이 응답은 클라이언트가 특정 채널에 대한 사용자 목록을 요청했을 때 서버가 전송하는 메시지입니다. 일반적으로 채널에 대한 사용자 목록을 클라이언트에게 제공하고 이를 알리는 데 사용됩니다.
  - `RPL_ENDOFNAMES`: 채널 사용자 목록의 끝을 나타내는 숫자 응답 코드입니다. 채널 사용자 목록이 끝났음을 나타내며, 클라이언트는 이를 받고 목록의 끝임을 알 수 있습니다. 이 응답은 RPL_NAMREPLY 메시지 다음에 사용되어 클라이언트에게 채널 사용자 목록의 끝을 표시합니다.

### LIST: channel과 topic 목록 나열

- 구성
  - 파라미터가 제시되면 그 채널들만 나열
  - private 채널은 “Prv” 라고 명시, secret 채널은 명시하지 않음(다만, 유저가 그 채널에 속해있으면 출력)

```jsx
LIST [<channel>{,<channel>} [<server>]]
```

- numerics
  - `ERR_NOSUCHSERVER`: 요청한 서버가 존재하지 않음을 나타냅니다.
  - `RPL_LISTSTART`: 채널 목록을 보여주기 시작함을 나타냅니다.
  - `RPL_LIST`: 채널 목록을 전송합니다.
  - `RPL_LISTEND`: 채널 목록 전송이 완료되었음을 나타냅니다.

### INVITE: 해당 채널에 클라이언트를 초대

- 구성
  - 대상 사용자가 초대되는 채널이 반드시 존재하거나 유효한 채널이어야 하는 요구 사항은 없다
  - 초대된 사용자를 초대 전용 채널 (모드 +i)로 초대하려면 초대를 보내는 클라이언트가 해당 채널에서 채널 운영자로 인식되어야 한다

```jsx
 INVITE <nickname> <channel>

 :Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel
                                   #Dust

   INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
                                   #Twilight_zone
```

- numerics
  - `ERR_NEEDMOREPARAMS`: 명령에 필요한 매개변수가 충분하지 않음을 나타냅니다.
  - `ERR_NOSUCHNICK`: 지정된 닉네임이 존재하지 않음을 나타냅니다.
  - `ERR_NOTONCHANNEL`: 클라이언트가 채널에 참여하지 않았음을 나타냅니다.
  - `ERR_USERONCHANNEL`: 사용자가 이미 채널에 참여 중임을 나타냅니다.
  - `ERR_CHANOPRIVSNEEDED`: 채널 운영자 권한이 필요함을 나타냅니다.
  - `RPL_INVITING`: 사용자에게 초대가 전송되었음을 나타냅니다.
  - `RPL_AWAY`: 사용자가 잠시 부재 중임을 나타냅니다.

### KICK: 해당 채널에서 클라이언트 강퇴

- 구성
  - 채널 운영자만 다른 사용자를 채널에서 추방할 수 있다
  - KICK 메시지를 수신한 각 서버는 해당 메시지가 유효한지 확인한다 (즉, 보낸 사람이 실제로 채널 운영자인지 확인) 그 후 해당 클라이언트를 강

```jsx
 KICK <channel> <user> [<comment>]
 KICK <channel>{,<channel>} <user>{,<user>} [<comment>]

 KICK &Melbourne Matthew         ; Kick Matthew from &Melbourne

KICK #Finnish John :Speaking English
                                ; Kick John from #Finnish using
                                "Speaking English" as the reason
                                (comment).

:WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                from channel #Finnish
```

- numerics
  - `ERR_NEEDMOREPARAMS`: 필요한 매개변수가 더 필요함
    `ERR_NOSUCHCHANNEL`: 채널이 존재하지 않음
    `ERR_BADCHANMASK`: 잘못된 채널 마스크
    `ERR_CHANOPRIVSNEEDED`: 채널 운영자 권한이 필요함
    `ERR_NOTONCHANNEL`: 채널에 참여하지 않음

## (3) Sending Messages

### PRIVMSG

- 구성
  - <receiver>는 메시지 수신자의 닉네임
    - <receiver>는 쉼표로 구분된 이름이나 채널 목록일 수도 있다.
  - <receiver> 매개변수는 호스트 마스크(#mask) 또는 서버 마스크($mask)일 수도 있다
    - 이 경우, 서버는 마스크와 일치하는 서버 또는 호스트를 가진 사용자에게만 PRIVMSG를 보냅니다. (마스크는 적어도 1개의 "."을 가져야 하며 마지막 "." 다음에 와일드카드가 없어야 합니다.)
    - 이러한 요구 사항은 "#_" 또는 "$_"로 메시지를 보내지 못하도록 함으로써 모든 사용자에게 방송되는 것을 방지한다
  - 이 확장된 PRIVMSG 명령은 운영자만 사용할 수 있다.
  ```jsx
  PRIVMSG <receiver>{,<receiver>} <전송할 텍스트>
  ```
- **코드 예시**
  - Angel에게 Wiz로부터 메시지를 보내는 경우:
    ```ruby
    :Angel PRIVMSG Wiz :안녕하세요, 이 메시지를 받고 있나요?
    ```
  - Angel에게 메시지를 보내고 Angel이 그것을 받았다는 응답을 받는 경우:
    ```
    PRIVMSG Angel :네, 받고 있어요!
    ```
- numeric replies
  - `ERR_NORECIPIENT:` 수신자가 지정되지 않았습니다.
  - `ERR_NOTEXTTOSEND`: 전송할 텍스트가 없습니다.
  - `ERR_CANNOTSENDTOCHAN`: 채널로 메시지를 보낼 수 없습니다.
  - `ERR_NOTOPLEVEL`: 최상위 도메인이 아닌 수신자에게 메시지를 보낼 수 없습니다.
  - `ERR_WILDTOPLEVEL`: 와일드카드가 포함된 최상위 도메인에 메시지를 보낼 수 없습니다.
  - `ERR_TOOMANYTARGET`S: 지정된 수신자가 너무 많습니다.
  - `ERR_NOSUCHNICK`: 해당하는 닉네임이 존재하지 않습니다.
  - `RPL_AWAY`: 수신자가 부재중임을 나타냅니다.

### NOTICE

- 구성
  - NOTICE 메시지는 PRIVMSG와 유사하게 사용된다
  - NOTICE와 PRIVMSG의 차이점은 NOTICE 메시지에 대한 자동 응답이 절대 전송되지 않아야 한다는 것이다
    - 이 규칙은 서버에도 적용되며, 서버는 NOTICE를 수신할 때 클라이언트에게 어떠한 오류 응답도 보내서는 안 됩니다.
    - 이 규칙의 목적은 클라이언트가 받은 것에 대한 응답을 자동으로 보내어 루프에 빠지지 않도록 하는 것이다
    - 이는 일반적으로 자동화된 시스템(클라이언트에 AI 또는 다른 대화형 프로그램이 제어하는)에서 사용되며, 항상 응답을 보이도록 설정되어 있어야 다른 자동화된 시스템과 루프에 빠지지 않는다

```jsx
NOTICE <nickname> <전송할 텍스트>
```

- numerics은 PRIVMSG와 동일

## (4) 추가 기능

### **PING**

- 구성
  - PING 메시지는 연결의 다른 쪽에 활성 클라이언트가 있는지 테스트하는 데 사용된다
  - 다른 활동이 감지되지 않는 경우 정기적으로 PING 메시지가 전송된다.
  - 연결이 설정된 시간 내에 PING 명령에 응답하지 않으면 해당 연결이 닫힌다.
  - PING 메시지를 수신한 모든 클라이언트는 가능한 빨리 <server1> (PING 메시지를 보낸 서버)에 적절한 PONG 메시지로 응답하여 여전히 살아 있음을 나타내어야 한다
  - 서버는 PING 명령에 응답해서는 안 되고 연결이 활성 상태임을 나타내기 위해 연결의 다른 쪽에서 PING을 응답하는 것에 의존해야 한다
  - <server2> 매개변수가 지정된 경우 PING 메시지가 해당 서버로 전달됩니다.

```jsx
PING <server1> [<server2>]
```

**숫자 응답**:

- ERR_NOORIGIN
- ERR_NOSUCHSERVER

**예시**:

- 서버가 다른 서버에게 살아 있다는 것을 나타내기 위해 PING 메시지를 보내는 경우:
  ```
  PING tolsun.oulu.fi

  ```
- 닉네임 WiZ에게 PING 메시지를 보내는 경우:
  ```
  PING WiZ

  ```

### **PONG**

- 구성
  PONG 메시지는 ping 메시지에 대한 응답입니다. <daemon2> 매개변수가 제공된 경우 이 메시지는 해당 데몬으로 전달되어야 합니다. <daemon> 매개변수는 PING 메시지에 응답하고 이 메시지를 생성한 데몬의 이름입니다.

```jsx
PONG <daemon> [<daemon2>]
```

**숫자 응답**:

- ERR_NOORIGIN
- ERR_NOSUCHSERVER

**예시**:

- PONG 메시지를 보낸 데몬이 csd.bu.edu이고, 이 메시지가 tolsun.oulu.fi로 전달되는 경우:
  ```
  PONG csd.bu.edu tolsun.oulu.fi

  ```
