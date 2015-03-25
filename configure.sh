#!/bin/bash

GCC_VERSION=$(g++ --version | grep ^g++ | sed 's/^.* //g')

if [[ ${GCC_VERSION} == "4.8"* ]]
then
    echo -e '\e[92mCompiler version OK.\e[0m';
else
    echo -e "\e[31mCompiler incompatibility. Current compiler version is \
${GCC_VERSION}. The version of the compiler this project was built against is \
4.8.2. Unexpected issues might occur.\e[0m"
fi


################################################################################
#                               GOOGLE_TEST                                    #
################################################################################
cd gtest-1.7.0/
echo 'About to configure GoogleTest.';
./configure || {
    echo -e '\e[31mFailed to configure GoogleTest.\e[0m'
    exit 1
}
echo -e '\e[92mSuccessfully configured GoogleTest.\e[0m';

echo 'About to build GoogleTest.';
make || {
    echo -e '\e[31mFailed to build GoogleTest.\e[0m'
    exit 1
}
echo -e '\e[92mSuccessfully built GoogleTest.\e[0m';

cd ..

################################################################################
#                                  NAUTY                                       #
################################################################################
cd nauty
echo 'About to configure NAUTY.';
./configure || {
    echo -e '\e[31mFailed to configure NAUTY.\e[0m'
    exit 1
}
echo -e '\e[92mSuccessfully configured NAUTY.\e[0m';

echo 'About to build NAUTY.';
make || {
    echo -e '\e[31mFailed to build NAUTY.\e[0m'
    exit 1
}
echo -e '\e[92mSuccessfully built NAUTY.\e[0m';

cd ..

################################################################################
#                               THIS PROJECT                                   #
################################################################################
echo 'About to build this project.';
make || {
    echo -e '\e[31mFailed to build this project.\e[0m'
    exit 1
}
echo -e '\e[92mSuccessfully built this project.\e[0m';
echo ''
echo "An easy way to check if everything is set up correctly is to run the \
script: ./run_unit_tests.sh"
