#!/usr/bin/env bash
programas=(ads preditor controle)
for i in ${programas[@]}
do
PID=`cat /home/ubuntu/git/embedded_emg_prosthesis/codigo-fonte/PC4/$i.pid`

if ! ps -p $PID > /dev/null
then
  rm /home/ubuntu/git/embedded_emg_prosthesis/codigo-fonte/PC4/$i.pid
  sudo ./$i.out & echo $! >>/home/ubuntu/git/embedded_emg_prosthesis/codigo-fonte/PC4/$i.pid
fi
done
