#Направете списък с директориите на вашата файлова система, до които нямате достъп.
#Понеже файловата система може да е много голяма, търсете до 3 нива на дълбочина.

find / -maxdepth 3 -type d >/dev/null 2> sol.txt; cat sol.txt | cut -d ':' -f 2 | tr -d ' ' | cut -c 4- | rev | cut -c 4- | rev; rm sol.txt