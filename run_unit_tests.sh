make || {
    echo -e "\e[31mFailed to compile.\e[0m"
    exit 1
}
./graph_test.exe || {
    echo -e "\e[31mFAILED graph_test\e[0m"
    exit 1
}
./graph_generator_test.exe || {
    echo -e "\e[31mFAILED graph_generator_test\e[0m"
    exit 1
}
./graph_utilities_test.exe || {
    echo -e "\e[31mFAILED graph_utilities_test\e[0m"
    exit 1
}
./nauty_wrapper_test.exe || {
    echo -e "\e[31mFAILED nauty_wrapper_test\e[0m"
    exit 1
}
