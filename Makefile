BUILD_DIR := build

.PHONY: all cpp py cpptest pytest clean

all: cpp py

$(BUILD_DIR)/CMakeCache.txt:
	cmake -B $(BUILD_DIR)

cpp: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) --target otagrum

py: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) --target otagrum_python

cpptest: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) --target tests
	cd $(BUILD_DIR) && ctest --output-on-failure -R '^cppcheck_'

pytest: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR) --target install
	cd $(BUILD_DIR) && ctest --output-on-failure -R '^pyinstallcheck_'

clean:
	rm -rf $(BUILD_DIR)
