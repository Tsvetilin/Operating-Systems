#!/bin/bash

read -p "Enter username: " username

if ! grep -q "${username}" /etc/passwd ; then
        echo "No such username exists"
        exit 1
fi

echo "Active sessions: " $(who | grep "${username}" | wc -l)

#Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от системата - след което извежда на стандартния изход колко активни сесии има потребителят в момента.