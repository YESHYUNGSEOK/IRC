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


### PASS

- 서버에 연결할 때 사용하는 비밀번호를 설정합니다.
- `PASS <password>`
- `ERR_NEEDMOREPARAMS` - Not enough parameters
- `ERR_ALREADYREGISTRED` - Already registered
- `ERR_PASSWDMISMATCH` - Password incorrect

```
PASS 대충비밀번호
```

### NICK

- 클라이언트의 닉네임을 설정합니다.
- `NICK <nickname>`
- `ERR_NONICKNAMEGIVEN` - No nickname given
- `ERR_ERRONEUSNICKNAME` - Erroneous nickname
- `ERR_NICKNAMEINUSE` - Nickname in use
- `ERR_NICKCOLLISION` - Nickname collision KILL(동시에 같은 닉네임을 사용하는 사용자가 있을 때 발생)

```
NICK Wiz	; Requesting the new nick "Wiz".

:WiZ NICK Kilroy	; WiZ changed his nickname to Kilroy.
:dan-!d@localhost NICK Mamoped	; dan- changed his nickname to Mamoped.
```

### USER

- 사용자 정보를 설정합니다.
- `USER <username> <hostname> <servername> <realname>`
- `ERR_NEEDMOREPARAMS` - Not enough parameters
- `ERR_ALREADYREGISTRED` - Already registered

#### 명령 예:
```
	USER guest 0 * :Ronnie Reagan
						; No ident server
						; User gets registered with username
						"~guest" and real name "Ronnie Reagan"

	USER guest 0 * :Ronnie Reagan
						; Ident server gets contacted and
						returns the name "danp"
						; User gets registered with username
						"danp" and real name "Ronnie Reagan"
```

### PING

- 연결이 살아 있는지 확인합니다.
- `PING <token>`
- `ERR_NEEDMOREPARAMS` - Not enough parameters
- `ERR_NOORIGIN` - No origin specified

클라이언트 연결 등록중에 처리 가능해야함.

### PONG

- `PONG [<server>] <token>`

서버 파라미터에 자신의 서버 이름을 넣어야함.

### OPER

- 클라이언트를 서버의 관리자로 설정합니다.
- `OPER <name> <password>`
- `ERR_NEEDMOREPARAMS` - Not enough parameters
- `ERR_NOOPERHOST` - Not vaild for your host
- `ERR_PASSWDMISMATCH` - Password incorrect
- `RPL_YOUREOPER` - You are now an IRC operator

### QUIT

- 서버에서 나갑니다.
- `QUIT [<reason>]`
- `ERROR` - Closing Link: <nick> (<reason>)

quit message가 없이 연결이 종료되면 사유에 맞게 적절한 기본 메시지를 사용해야함.

### ERROR

- 서버에서 오류 메시지를 보냅니다.
- `ERROR <reason>`

연결 종료

## 일단 여기까지만 정리


### JOIN

- 채널에 참여합니다.
- `JOIN <channel>{,<channel>} [<key>{,<key>}]`
- `ERR_NEEDMOREPARAMS` - Not enough parameters
- `ERR_BADCHANMASK`
- `ERR_INVITEONLYCHAN`
- `ERR_BANNEDFROMCHAN`
- `ERR_BADCHANNELKEY`
- `ERR_CHANNELISFULL`
- `ERR_BADCHANMASK`
- `ERR_NOSUCHCHANNEL`

### PART

- 채널에서 나갑니다.
- `PART <channel>{,<channel>} [<Part Message>]`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHCHANNEL`
- `ERR_NOTONCHANNEL`

### MODE

- 채널이나 사용자의 모드를 설정합니다.
- `MODE <channel> <modestring> [<mode arguments>]`
- `MODE <nickname> <modestring> [<mode arguments>]`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHNICK`

### TOPIC

- 채널의 주제를 설정합니다.
- `TOPIC <channel> [<topic>]`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOTONCHANNEL`

### NAMES

- 채널에 있는 사용자 목록을 보여줍니다.
- `NAMES [<channel>{,<channel>}]`
- `ERR_NOSUCHSERVER`
- `ERR_NONICKNAMEGIVEN`

### LIST

- 채널 목록을 보여줍니다.
- `LIST [<channel>{,<channel>}]`
- `ERR_NOSUCHSERVER`

### KICK

- 채널에서 사용자를 강제로 내보냅니다.
- `KICK <channel> <user> [<comment>]`
- `ERR_NEEDMOREPARAMS`
- `ERR_NOSUCHCHANNEL`
- `ERR_NOTONCHANNEL`
- `ERR_USERNOTINCHANNEL`

### PRIVMSG

- 사용자나 채널에 메시지를 보냅니다.
- `PRIVMSG <receiver> <text to be sent>`
- `ERR_NORECIPIENT`
- `ERR_NOTEXTTOSEND`

### NOTICE

- 사용자나 채널에 메시지를 보냅니다.
- `NOTICE <nickname> <text>`
- `NOTICE <channel> <text>`
- `ERR_NORECIPIENT`
- `ERR_NOTEXTTOSEND`

### QUIT

- 서버에서 나갑니다.
- `QUIT [<Quit Message>]`

### MOTD

- 서버의 메시지를 보여줍니다.
- `MOTD`
- `ERR_NOMOTD`