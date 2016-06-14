all:
	grunt
	pebble build

aplite:
	pebble install --emulator aplite --logs

basalt:
	pebble install --emulator basalt --logs

chalk:
	pebble install --emulator chalk --logs
