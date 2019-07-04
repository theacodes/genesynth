.PHONY: format
format:
	clang-format -i firmware/**/*.h firmware/**/*.cpp firmware/**/*.ino simulator/src/*.h simulator/src/*.cpp simulator/src/**/*.h simulator/src/**/*.cpp
