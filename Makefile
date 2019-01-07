.PHONY: format
format:
	clang-format -i firmware/**/*.h firmware/**/*.cpp firmware/**/*.ino
