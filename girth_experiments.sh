#!/bin/bash
make || {
    echo -e "\e[31mFailed to compile.\e[0m"
    exit 1
}
for i in {3..20}
do
  ./callgeng_generic_girth.exe ${i} #> results/extremal_girth_16_order_${i}.txt
  echo "Order ${i}"
  #head -1 results/extremal_girth_16_order_${i}.txt
done
