make || {
    echo -e "\e[31mFailed to compile.\e[0m"
    exit 1
}
./graph_test
