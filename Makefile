.PHONY: format
format:
	clang-format -i firmware/genesynth/*.h firmware/genesynth/*.cpp firmware/genesynth/*.ino
