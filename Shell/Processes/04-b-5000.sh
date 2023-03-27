#Намерете колко физическа памет заемат всички процеси на потребителската група root.

ps -e -g root -o rss= | awk '{sum+=$1} END {print sum}'

#or

ps -e -g root -o rss= |sed -E 's/.*/+ &/g' | xargs | tail -c +3 | bc