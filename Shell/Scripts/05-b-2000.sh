#Да се напише shell скрипт, който приканва потребителя да въведе низ (име) и изпечатва "Hello, низ".

#!/bin/bash

read -p "Enter your name: " name
echo "Hello, ${name}"