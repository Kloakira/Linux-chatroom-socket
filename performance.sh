#!/bin/bash  
#得到进程的 ID
function GetPID #User #Name 
 { 
    PsUser=$1 
    PsName=$2 
    pid=`ps -u $PsUser|grep $PsName|grep -v grep|grep -v vi|grep -v dbx\n |grep -v tail|grep -v start|grep -v stop |sed -n 1p |awk '{print $1}'` 
    echo $pid 
 }

PID=`GetPID root server` 
 
    echo -e "server进程 PID: $PID .\n"


# 检查进程是否存在
if [ "-$PID" == "-" ] 
then 
 { 
    echo "The process does not exist."
 } 
fi

date +"%Y-%m-%d %H:%M:%S"  >> server.log
#对业务进程 CPU 进行实时监控
function GetCpu 
 { 
   CpuValue=`ps -p $1 -o pcpu |grep -v CPU ` 
   echo $CpuValue 
 }
#判断 CPU 利用率是否超过限制
function CheckCpu 
 { 
    PID=$1 
    cpu=`GetCpu $PID` 
    if [ $cpu > 90 ] 
    then 
    { 
 		echo "server进程 CPU 利用率 is larger than 90%:$cpu %."
    } 
    fi 
    echo $cpu
 }
pcpu=`GetCpu $PID`
 #对业务进程内存使用量进行监控
function GetMem 
 { 
    MEMUsage=`ps -o vsz -p $1|grep -v VSZ` 
    (( MEMUsage /= 1000)) 
    echo $MEMUsage 
 }

mem=`GetMem $PID` 

echo -e "=====server进程 CPU 利用率: $pcpu %.======\n=====server进程内存使用量:$mem M.  ======\n" 
echo -e "=====server进程 CPU 利用率: $pcpu %.======\n=====server进程内存使用量:$mem M.  ======\n" >> server.log
#判断内存使用是否超过限制
 if [ $mem -gt 500 ] 
 then 
 { 
     echo "server进程内存使用量超过 500M"
 } 
 fi

 # 端口是否在监听
 function Listening 
 { 
    TCPListeningnum=`netstat -an | grep ":$1 " | awk '$1 == "tcp" && $NF == "LISTEN" {print $0}' | wc -l` 
    UDPListeningnum=`netstat -an|grep ":$1 " |awk '$1 == "udp" && $NF == "0.0.0.0:*" {print $0}' | wc -l` 
    (( Listeningnum = TCPListeningnum + UDPListeningnum )) 
    if [ $Listeningnum == 0 ] 
    then 
    { 
        echo "0"
    } 
    else 
    { 
        echo "1"
    } 
    fi 
 }

 isListen=`Listening 10222` 
 #查看某个进程名正在运行的个数
 Runnum=`ps -ef | grep -v vi | grep -v tail | grep "[ /]client" | grep -v grep | wc -l`

    if [ $isListen -eq 1 ] 
    then 
    { 
        echo -e "**端口 10222 在监听**\n**client进程正在运行的个数: $Runnum**\n"
    } 
    else 
    { 
        echo "**端口 10222 没有在监听**\n**client进程正在运行的个数: $Runnum**\n"
    } 
    fi


 #检测系统 CPU 负载
 function GetSysCPU 
 { 
   CpuIdle=`vmstat 1 5 |sed -n '3,$p' |awk '{x = x + $15} END {print x/5}' |awk -F. '{print $1}'`
   CpuNum=`echo "100-$CpuIdle" | bc` 
   echo $CpuNum 
 }
 function GetSysMem 
{   
  Mem=`free -m | grep '内存' | awk '{print $3}'`  
  echo $Mem  
} 

 cpu=`GetSysCPU` 
 mem=`GetSysMem`
 if [ $cpu -gt 90 ] 
 then 
 { 
    echo "CPU 利用率超过 90%"
 } 
 fi

 echo -e "=====系统 CPU 利用率: $cpu %.\t  ======\n=====系统内存使用量: $mem M.\t  ======\n" 
 echo -e "=====系统 CPU 利用率: $cpu %.\t  ======\n=====系统内存使用量: $mem M.\t  ======\n" >> server.log