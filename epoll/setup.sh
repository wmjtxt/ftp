#default_color white
#double_buffer yes
#no_buffers yes
#update_interval 1
#total_run_times 0
#own_window yes
#own_window_type normal
#own_window_transparent yes
#own_window_hints undecorated,below,sticky,skip_taskbar,skip_pager
#
#own_window_colour 000000
#own_window_argb_visual yes
#own_window_argb_value 0
#
#double_buffer yes
#draw_shades yes
#draw_outline no
#draw_borders no
#draw_graph_borders n0
#minimum_size 296 5
#maximum_width 406
#default_color ffffff
#default_shade_color 000000
#default_outline_color 000000
#alignment top_right
#gap_x 60
#gap_y 100
#cpu_avg_samples 2
#override_utf8_locale no
#uppercase no # set to yes if you want all text to be in uppercase
#use_spacer no
#use_xft yes
#xftfont WenQuanYi Zen Hei:pixelsize=12
#override_utf8_locale yes
#use_spacer no
#minimum_size 296 5
#TEXT
#${font LCD:style=Bold:pixelsize=50}${alignc}${time %H:%M:%S}${font WenQuanYi Zen Hei:pixelsize=18}
#
#${time %Y年%b%d日 星期%a}${alignc}
#
#${font WenQuanYi Zen Hei:pixelsize=30}${exec cat $HOME/.feed/feed_wd}${font WenQuanYi Zen Hei:pixelsize=18}  ${exec cat $HOME/.feed/feed_ls}
#${font WenQuanYi Zen Hei:pixelsize=14}${exec cat $HOME/.feed/feed_xls}
#${font WenQuanYi Zen Hei:pixelsize=12}
#${color #98c2c7}主机名: ${color}$alignr$nodename
#${color #98c2c7}内核:  ${color}$alignr$kernel
#${color #98c2c7}已开机: ${color}$alignr$uptime
#
#${color #98c2c7}CPU使用率: ${color}$cpu%
#
#$cpubar
#
# CPU1 温度: $alignr${exec sensors | grep 'Core 0' | cut -c17-25}
# CPU2 温度: $alignr${exec sensors | grep 'Core 1' | cut -c17-25}
#
#${color #98c2c7}CPU占用:${color}
#
# ${top name 1} ${alignr}${top cpu 1} 
# ${top name 2} ${alignr}${top cpu 2} 
# ${top name 3} ${alignr}${top cpu 3} 
# ${top name 4} ${alignr}${top cpu 4} 
# ${top name 5} ${alignr}${top cpu 5} 
# ${top name 6} ${alignr}${top cpu 6} 
# ${top name 7} ${alignr}${top cpu 7} 
# ${top name 8} ${alignr}${top cpu 8} 
#
#${color #98c2c7}物理内存：${color}${alignr}$mem / $memmax
#
#${membar}
#
#${color #98c2c7}根分区: ${color}${alignr}${fs_free /} / ${fs_size /}
#
#${fs_bar 4 /}
#
#${color #98c2c7}Home分区: ${color}${alignr}${fs_free /home} / ${fs_size /home}
#
#${fs_bar 4 /home}
#
#${color #98c2c7}网络使用:${color}
#
#下载速度:$alignr${downspeed wlp2s0} k/s
#上传速度:$alignr${upspeed wlp2s0} k/s
#${downspeedgraph wlp2s0 324D23 77B753}
#
#
#
#
#
#
##!/bin/bash
#if test "$1" = 'network'
#then
#	net=`route | grep default | awk '{print $NF}'`
#	cat $HOME/.conkyrc | sed -i "/下载速度/d" $HOME/.conkyrc 
#	cat $HOME/.conkyrc | sed -i "/上传速度/d" $HOME/.conkyrc
#	cat $HOME/.conkyrc | sed -i "/downspeedgraph/d" $HOME/.conkyrc
#	echo "下载速度:\$alignr\${downspeed $net} k/s" >> $HOME/.conkyrc
#	echo "上传速度:\$alignr\${upspeed $net} k/s" >> $HOME/.conkyrc
#	echo "\${downspeedgraph $net 324D23 77B753}" >> $HOME/.conkyrc
#	exit 0
#fi
#killall conky
#cat $HOME/.profile | sed -i '/conky/d' $HOME/.profile
#cat $HOME/.profile | sed -i '/feed.sh/d' $HOME/.profile
#
#rm -fr $HOME/.conkyrc $HOME/.feed
#cp -rf conkyrc  $HOME/.conkyrc  
#cp -rf feed $HOME/.feed 
#echo 'conky &' >> $HOME/.profile
#echo "sh $HOME/.feed/feed.sh &" >> $HOME/.profile
#
#conky 2>/dev/null &
#sh $HOME/.feed/feed.sh && rm -fr $HOME/Desktop/conky
#
#
#
#
#
#
#cd $HOME/.feed/
#rm -fr feed_*
#echo '正在获取位置信息  ....' > feed_ls
#sleep 4
#echo '拉取天气信息,请稍后  ....' > feed_ls
#sleep 5
#echo '截取字符串并效验信息准确性' > feed_ls
#echo '   请稍后  ....' >> feed_ls
#sleep 5
#echo '正在完成最后的配置  ....' > feed_ls
#sleep 5
#a=111 ; b=113 ; c=122 ; d=155 ; e=117 ; f=118
#wget -q https://tianqi.moji.com/ -O tqym
#pd=`head -108 tqym | tail -1 | grep li`
#if test "$pd" != ''
#then	
#	a=121 ; b=123 ; c=132 ; d=165 ; e=127 ; f=128
#	pd=`head -118 tqym | tail -1 | grep li`
#	if test "$pd" != ''
#	then
#	a=131 ; b=133 ; c=142 ; d=175 ; e=137 ; f=138
#	fi
#
#fi
#wd=`head -$a tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}'`
#tq=`head -$b tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' `
#by=`head -$c tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}'`
#wc=`head -$d tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' `
#dq=`head -72 tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}'`
#kqzl=` head -105 tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' | awk '{print $1 $2}'`
#sd=` head -$e tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' | awk '{print $1 $2}'`
#fl=`head -$f tqym | tail -1 | awk -F'>' '{print $2}' | awk -F'<' '{print $1}'`
#
#rm -fr tqym
#echo "$wd°" > feed_wd 
#echo "  $tq     $wc" > feed_ls
#echo "空气质量：$kqzl    $fl     $sd" > feed_xls
#echo "$by" >> feed_xls
#cw=`cat feed_wd` 2>/dev/null
#cl=`cat feed_ls` 2> /dev/null
#cx=`cat feed_xls` 2>/dev/null
#if [ "$cw" = '' ] || [ "$cl" = '' ] || [ "$cx" = '' ]
#then
#	sz=`cat sz`
#	if test $sz -lt '9'
#	then
#		sleep 1
#		sz=`expr $sz + 1`
#		echo $sz > sz
#		sh $0
#	else
#		exit 0
#	fi
#fi
#echo 1 > sz
#
#
tj=`pwd | awk -F/ '{print $NF}'`
if test "$tj" != 'Desktop' || "$tj" != '桌面'
then
	echo '只有在桌面目录下才能执行哦：错误！！！'
	echo '执行：cd $HOME/Desktop  到桌面目录后，在重新执行本程序'
	exit
fi
tj=`ls | grep setup.sh`
if test "$tj" != 'setup.sh'
then
	echo '没有找到setup.sh可执行程序：错误！！！'
	echo '请将下载好的压缩包解压到桌面后得到setup.sh'
	exit
fi

mkdir conky
mkdir conky/feed
head -83 setup.sh |  sed 's/.//1' > conky/conkyrc
head -113 setup.sh | tail -24 | sed 's/.//1' > conky/setup.sh
head -174 setup.sh | tail -55 | sed 's/.//1' > conky/feed/feed.sh
sudo  apt install  conky hddtemp curl lm-sensors conky-all -y
sudo chmod u+s /usr/sbin/hddtemp 
sudo sensors-detect
cd conky
sh setup.sh
sh setup.sh network

