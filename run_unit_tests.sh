#!/bin/bash
make || {
    echo -e "\e[31mFailed to compile.\e[0m"
    exit 1
}
for i in {1..5}
do
    ./graph_test.exe || {
        echo -e "\e[31mFAILED graph_test\e[0m"
        exit 1
    }
    ./graph_utilities_test.exe || {
        echo -e "\e[31mFAILED graph_utilities_test\e[0m"
        exit 1
    }
    ./girth_5_graph_test.exe || {
        echo -e "\e[31mFAILED girth_5_graph_test\e[0m"
        exit 1
    }
    ./graph_generator_test.exe || {
        echo -e "\e[31mFAILED graph_generator_test\e[0m"
        exit 1
    }
    ./canonical_graph_generator_test.exe || {
        echo -e "\e[31mFAILED canonical_graph_generator_test\e[0m"
        exit 1
    }
    ./nauty_wrapper_test.exe || {
        echo -e "\e[31mFAILED nauty_wrapper_test\e[0m"
        exit 1
    }
done
