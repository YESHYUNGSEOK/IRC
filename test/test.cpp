#include "CommandHandler.hpp"
#include "Message.hpp"
#include "NumericReply.hpp"

int main(void) {
  std::string case1 = "PRIVMSG Angel :yes I'm receiving it !\r\n";
  std::string case2 = "JOIN #foobar\r\n";
  std::string case3 = "MODE #Finnish +t :dune 2 \r\n";

  Message msg1(case1);
  Message msg2(case2);
  Message msg3(case3);

  msg1.parseMsg();
  msg2.parseMsg();
  msg3.parseMsg();

  std::cout << "======================" << std::endl;
  std::cout << "Case 1: " << std::endl;
  std::cout << msg1.get_command() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg1.get_params().begin();
       it != msg1.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }
  std::cout << "======================" << std::endl;
  std::cout << "Case 2: " << std::endl;
  std::cout << msg2.get_command() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg2.get_params().begin();
       it != msg2.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }
  std::cout << "======================" << std::endl;
  std::cout << "Case 3: " << std::endl;
  std::cout << msg3.get_command() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg3.get_params().begin();
       it != msg3.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }
  std::cout << "======================" << std::endl;
}
