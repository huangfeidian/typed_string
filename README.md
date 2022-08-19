# typed_string
一个带有格式说明的异构容器，可以与字符串相互转换。
## typed_string_desc
`typed_string_desc` 就是字符串的类型描述， 支持如下几种基础类型：
1. `str` 字符串类型 
2. `bool` bool值类型 取值要么是1 要么是0
3. `uint` 无符号整数
4. `int` 带符号整数
5. `float` 浮点数

然后对于基础类型，我们可以增加他的取值范围限定，方式为`{"xxx": [a, b, c]}`,这里的xxx就是基础类型名字， 而`a,b,c`则是可以供选择的值，其类型为`xxx`:
1. `{"int": [1, 2]}` 
2. `{"str": ["A", "B", "C"]}`
3. `{"float": [1.0, 2.0]}`

在这些基础类型之上我们提供了两种组合类型：
1. `tuple`类型，可以理解为结构体，声明方式为`[A, B, C]`, 这里的`A,B,C`数量为任意多个，且每个元素都是一个有效的`typed_string_desc`,例子`["float", "int", {"int": [1, 2]}]`
2. `list`类型，可以理解为数组， 声明方式为`[A, n]`，这里的`A`是一个有效的`typed_string_desc`,而`n`是一个非负整数，表示数组的大小， 如果`n==0`，则表明是一个不限制大小的数组，例子`["int", 2], [["int", "float"], 0]`


对于更多的`typed_string_desc`的例子参见`test`目录下的`parse_test.cpp`里的测试样例





## 安装与使用
当前项目支持`cmake`的包管理，安装的时候只需要如下几步：
1. `mkdir build`
2. `cd build`
3. `cmake .. -DCMAKE_INSTALL_PREFIX=<YOUR DIR>`
4. `cmake --build ./ --target install`
如上四步之后，整个`package`就会安装在`<YOUR DIR>` 目录下

如果有其它项目依赖于这个项目，则需要在`CMakeLists`里增加如下几行：
1. `find_package(typed_string CONFIG REQUIRED)`
2. `target_link_libraries(<YOUR EXE> typed_string)`



