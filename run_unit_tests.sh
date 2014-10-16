make || {
    echo -e "\e[31mFailed to compile.\e[0m"
    exit 1
}
./graph_test.exe
./graph_generator_test.exe
./nauty_wrapper_test.exe
