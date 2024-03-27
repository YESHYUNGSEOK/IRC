
#include "Message.hpp"

int main(void) {
  std::string case1 = "PRIVMSG Angel :yes I'm receiving it !\r\n";
  std::string case2 = "JOIN #foobar\r\n";
  std::string case3 = "MODE #Finnish +t :dune 2 \r\n";
  std::string case4 = "CAP LS \r\n";
  std::string case5 = "CAP ::-)\r\n";
  std::string case6 = "CAP :\r\n";

  Message msg1(case1);

  std::cout << "======================" << std::endl;
  std::cout << "Case 1: " << std::endl;
  std::cout << msg1.get_command() << std::endl;
  std::cout << "param_size: " << msg1.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg1.get_params().begin();
       it != msg1.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  Message msg2(case2);

  std::cout << "======================" << std::endl;
  std::cout << "Case 2: " << std::endl;
  std::cout << msg2.get_command() << std::endl;
  std::cout << "param_size: " << msg2.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg2.get_params().begin();
       it != msg2.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  Message msg3(case3);

  std::cout << "======================" << std::endl;
  std::cout << "Case 3: " << std::endl;
  std::cout << msg3.get_command() << std::endl;
  std::cout << "param_size: " << msg3.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg3.get_params().begin();
       it != msg3.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  Message msg4(case4);

  std::cout << "======================" << std::endl;
  std::cout << "Case 4: " << std::endl;
  std::cout << msg4.get_command() << std::endl;
  std::cout << "param_size: " << msg4.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg4.get_params().begin();
       it != msg4.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  Message msg5(case5);

  std::cout << "======================" << std::endl;
  std::cout << "Case 5: " << std::endl;
  std::cout << msg5.get_command() << std::endl;
  std::cout << "param_size: " << msg5.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg5.get_params().begin();
       it != msg5.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  Message msg6(case6);

  std::cout << "======================" << std::endl;
  std::cout << "Case 6: " << std::endl;
  std::cout << msg6.get_command() << std::endl;
  std::cout << "param_size: " << msg6.get_params().size() << std::endl;
  for (std::vector<std::string>::const_iterator it = msg6.get_params().begin();
       it != msg6.get_params().end(); it++) {
    std::cout << *it << std::endl;
  }

  std::cout << "======================" << std::endl;
  return 0;
}
