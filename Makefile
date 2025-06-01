# 컴파일러 및 옵션
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lsqlite3

# 소스 및 오브젝트
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

# 실행 파일 이름
TARGET = football_manager

# 기본 타겟
all: $(TARGET)

# 링크
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# 개별 .cpp → .o 규칙
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 정리
clean:
	rm -f $(OBJS) $(TARGET)

# 실행
run: $(TARGET)
	./$(TARGET)
