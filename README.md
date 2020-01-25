# typed_string
一个带有格式说明的异构容器，可以与字符串相互转换。
## typed_string_desc
`typed_string_desc` 就是字符串的类型描述， 支持如下几种类型：
1. `comment` 注释类型 就是简单的string 但是不参与值类型表示
2. `str` 字符串类型 
3. `bool` bool值类型 取值要么是1 要么是0
4. `uint32` 无符号32位整数
5. `int` 带符号32位整数
6. `uint64` 无符号64位整数
7. `int64` 带符号64位整数
8. `float` 单精度浮点数
9. `double` 双精度浮点数
10. `choice_int` 整数枚举类型，规定可以取的int值列表，`choice_int(1, 2, 3)`代表符合这个schema的值一定是1，2，3这三个整数中的一个
11. `choice_str` 字符串枚举类型，规定可以取的str值列表，`choice_str(false,true)` 代表符合这个schema的值一定是false true中的其中一个
12. `list` 列表类型 可以包含有限个或者无限个同类型的值，值之间可以指定分隔符，默认分隔符为逗号。
    1. `list(int, 3)` 代表三个int，
    2. `list(str, 4, #)` 代表四个以#相分割的字符串
    3. `list(int, 0)` 代表不确定个数的int值，以逗号相分割
13. `tuple` 元组类型， 可以包含有限个不同类型的值，值之间可以指定分割符
    1. `tuple(int, str) `两个元素的元组，一个是整数，一个是字符串，两个值之间用逗号分隔
    2. `tuple(int, int, str)` 三元元素的元组，头两个是整数，最后一个是字符串， 值之间用逗号分隔
    3. `tuple(str, str, #)` 两个字符串组成的元组，分隔符为#

这里的list和tuple是可以嵌套其他类型的，有如下例子:
1. `list(choice_int(0,1,2), 3)`
2. `tuple(choice_str(true, false), bool)`
3. `tuple(list(int, 3), bool, #)`
嵌套多层的时候需要注意不同层级之间的分隔符不要冲突，如果冲突了，则会导致这个`schema`的`parse`失败。在多层`tuple,list`的情况下需要手工指定不同的分隔符。
下面是`typed_string_desc`的基本结构体
```c++
struct typed_string_desc
{
    using choice_int_detail_t = std::pair<int*, std::uint32_t>;
    using choice_str_detail_t = std::pair<std::string_view*, std::uint32_t>;
    using list_detail_t = std::tuple<typed_string_desc*, std::uint32_t, char>;// detail_type length <seperator>
    using tuple_detail_t = std::tuple<typed_string_desc**, std::uint32_t, char>;//<type1, type2, type3> seperator
    basic_value_type _type;
    std::variant<choice_int_detail_t, choice_str_detail_t, tuple_detail_t, list_detail_t> _type_detail;
}
```

对于更多的`typed_string_desc`的例子参见`test`目录下的`parse_test.cpp`里的测试样例

## typed_string_value
`typed_string_value`是带`schema`的异构值类型，它可以在特定`schema`的指导下从字符串中`parse`出来，也可以转换为字符串。
为了内存管理方便，这里我们定义了一个`arena_typed_string`来作为`typed_string_value`的具体载体。
```c++
struct arena_typed_vec
{
    arena_typed_value** p_value;
    std::uint32_t size;
};
class arena_typed_value
{
public:
    
    const typed_string_desc* type_desc;
    const spiritsaway::memory::arena* arena;
    union {
        bool v_bool;
        std::uint32_t v_uint32; 
        std::int32_t v_int32;
        std::uint64_t v_uint64;
        std::int64_t v_int64;
        float v_float;
        double v_double;
        std::string_view v_text;
        arena_typed_vec v_vec; 
    };
}
```
这里的`memory::arena`负责构造`arena_typed_value`时的内存分配，只分配，不释放，要释放的时候需要所有一起都释放掉。
在这个`arena_typed_value`上，我们提供了几个转换函数，来获取所期待的类型的值：

```c++
template <typename T> 
std::optional<T> expect_simple_value() const;
template <typename T>
std::optional<T> expect_value() const;
template <typename... args>
std::optional<std::tuple<args ...>> expect_tuple_value() const;
```

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



