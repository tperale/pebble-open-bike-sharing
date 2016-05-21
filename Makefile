all:
	grunt
	pebble build

aplite:
	pebble install --emulator aplite
	pebble logs --emulator aplite
