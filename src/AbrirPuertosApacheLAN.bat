@echo off
echo ---------------------------------------------
echo    *** Abriendo puertos Apache LAN ***
echo ---------------------------------------------

:: Variables
set TCPPORT=7777
set UDPPORT1=7778
set UDPPORT2=7779

:: TCP - Servidor escucha en 7777
netsh advfirewall firewall add rule name="ApacheLAN_TCP_Server" dir=in action=allow protocol=TCP localport=%TCPPORT%

:: UDP - PC1 escucha en 7778 (envía a 7779)
netsh advfirewall firewall add rule name="ApacheLAN_UDP_Recv" dir=in action=allow protocol=UDP localport=%UDPPORT1%

:: UDP - PC2 escucha en 7779 (envía a 7778)
netsh advfirewall firewall add rule name="ApacheLAN_UDP_Recv2" dir=in action=allow protocol=UDP localport=%UDPPORT2%

:: Permitir salida (opcional, pero recomendado)
netsh advfirewall firewall add rule name="ApacheLAN_TCP_Out" dir=out action=allow protocol=TCP localport=%TCPPORT%
netsh advfirewall firewall add rule name="ApacheLAN_UDP_Out" dir=out action=allow protocol=UDP localport=%UDPPORT1%
netsh advfirewall firewall add rule name="ApacheLAN_UDP_Out2" dir=out action=allow protocol=UDP localport=%UDPPORT2%

echo ---------------------------------------------
echo   Puertos TCP %TCPPORT% y UDP %UDPPORT1%/%UDPPORT2% abiertos!
echo ---------------------------------------------
pause
