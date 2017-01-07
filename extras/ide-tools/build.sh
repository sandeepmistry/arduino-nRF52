#!/bin/sh -x

# search for arduino IDE: OSX, Linux:opt, Debian, EPEL
for IDE_LIB_PATH in /Applications/Arduino.app/Contents/Java /opt/arduino/lib /usr/share/arduino/lib/ /usr/share/arduino; do
	if [ -e "${IDE_LIB_PATH}/pde.jar" ]; then
		if [ -f "${IDE_LIB_PATH}/arduino-core.jar" ]; then
			ARDUINO_CORE="arduino-core.jar"
		else
			ARDUINO_CORE="core.jar"
		fi
		break
	fi
done

PATH="$JAVA_HOME:$PATH"

rm -rf bin
mkdir -p bin
javac -target 1.8 -cp "${IDE_LIB_PATH}/pde.jar:${IDE_LIB_PATH}/${ARDUINO_CORE}" -d bin *.java && jar cvf nRF5FlashSoftDevice.jar -C bin .
