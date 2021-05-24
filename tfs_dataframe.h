/**
 * @file tfs_dataframe.h
 * @author awegsche (you@domain.com)
 * @brief A self contained TFS dataframe.
 *
 * Features:
 *  - Can load from and save to TFS files
 *  - Gives low level access to the contents of a TFS file
 *      (no actions like sum / standard deviation etc.)
 *  - Supports a home brewn binary file format
 *  - no zlib compression of files yet
 *
 * @version 1.0
 * @date 2021-03-08
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iterator>
#include <complex>
#include <variant>

namespace tfs
{
typedef uint16_t size_length_t;

/**
     * @brief Helper for reading writing
     *
     */
namespace helper {
/**
         * @brief Write a string to a binary `std::ostream`
         *
         * @param stream the stream
         * @param str the string to be written
         */
inline void write_string(std::ostream& stream, const std::string& str){
    size_length_t s = static_cast<size_length_t>(str.size());
    stream.write(reinterpret_cast<const char*>(&s), sizeof(size_length_t));
    stream.write(reinterpret_cast<const char*>(&str[0]), s);
}

/**
     * @brief Read a `std::string` from the binary stream `stream`
     *
     * @param stream the stream
     * @return std::string
     */
inline std::string read_string(std::istream& stream) {
    std::string str;
    size_length_t s;
    stream.read(reinterpret_cast<char*>(&s), sizeof(size_length_t));
    size_t size = static_cast<size_t>(s);
    str.resize(size);
    stream.read(reinterpret_cast<char*>(&str[0]), size);
    return str;
}
}
/**
     * @brief Width for printing
     *
     */
constexpr int FIELDWIDTH = 15;

/**
     * @brief The TFS data types
     *
     */
enum DataType {
    S,  // string
    LE, // float (double)
    D,  // int (decimal?)
    B,  // bool
    C,  // complex
};

/**
     * @brief A kind of variant (but restricted to TFS data types)
     * Convenience constructors from the corresponding types exist.
     *
     * Note: numbers are internally represented as either `float` or `double`
     *
     * @tparam real
     */
template<typename real>
struct data_value {
    DataType type;
    std::variant<int, real, bool, std::complex<real>, std::string> payload;

    data_value() :type(DataType::D) { payload = 0;}
    data_value(int i) :type(DataType::D) { payload = i;}
    data_value(double d) :type(DataType::LE) { payload = static_cast<real>(d);}
    data_value(bool b) :type(DataType::B) { payload = b;}
    data_value(const std::string& s) : type(DataType::S), payload(s) {}
    data_value(const char* s) : type(DataType::S), payload(std::string(s)) {}


    std::string pretty_print() const {
        std::ostringstream ss;
        switch (type)
        {
        case DataType::D:
            ss << get_int();
            break;
        case DataType::LE:
            ss << get_double();
            break;
        case DataType::S:
            return get_string();
        case DataType::C:
            ss << get_complex();
            break;
        }
        return ss.str();
    }

    void write_to_binary(std::ostream& file) const {
        file.write(reinterpret_cast<const char*>(&this->type), sizeof(DataType));
        switch (type) {
        case DataType::D:
            file.write(reinterpret_cast<const char*>(&get_int()), sizeof(int));
            break;
        case DataType::LE:
            file.write(reinterpret_cast<const char*>(&get_double()), sizeof(real));
            break;
        case DataType::S:
        {
            helper::write_string(file, get_string());
            break;
        }
        }
    }

    static data_value read_from_binary(std::istream& file) {
        DataType t;
        file.read(reinterpret_cast<char*>(&t), sizeof(DataType));

        switch (t) {
        case DataType::S:
        {
            auto str = helper::read_string(file);
            return data_value(str);
        }
        case DataType::LE: {
            real r;
            file.read(reinterpret_cast<char*>(&r), sizeof(real));
            return data_value(r);
        }

        default:
            throw std::runtime_error("not implemented");
            break;
        }
        return data_value(0.0);
    }

    /**
     * @brief Returns the internal value as int.
     * Throws an error if the type is not `%d`.
     *
     * @return int
     */
    const int& get_int() const {
        return std::get<int>(payload);
    }

    int& get_int() {
        return const_cast<int&>(static_cast<const data_value<real>&>(*this).get_int());
    }

    /**
     * @brief Returns the internal value as double (either `float` or `double`).
     * Throws an error if the type is not `%le`.
     *
     * @return double
     */
    const real& get_double() const {
        return std::get<real>(payload);
    }
    real& get_double() {
        return const_cast<real&>(static_cast<const data_value<real>&>(*this).get_double());
    }

    /**
     * @brief Returns the internal value as `std::complex`.
     * Throws an error if the type is neither `%c` nor `%le`.
     *
     * Note: Converts an `%le` value silently to `std::complex`.
     *
     * @return std::complex
     */
    std::complex<real> get_complex() const {
        if (type == DataType::C) return std::get<std::complex<real>>(payload);
        if (type == DataType::LE)
            return std::complex<real>(std::get<real>(payload), 0.0);
    }

    /**
     * @brief Returns the internal value as string.
     * Throws an error if the type is not `%s`.
     *
     * @return std::string
     */
    const std::string& get_string() const {
        return std::get<std::string>(payload);
    }
};


/**
     * @brief Wrapper of TFS properties. A named `data_value`.
     *
     * @tparam real
     */
template<typename real>
struct data_property {
    data_value<real> value;
    std::string name;

    template<typename T>
    data_property(const std::string& name, const T& value)
        :name(name), value(value) {}

    data_property(const std::string& name, const data_value<real>& value)
        : name(name), value(value) {}

    data_property(const std::string& name, data_value<real>&& value)
        : name(name), value(std::move(value)) {}

    void write_to_binary(std::ostream& file) const {
        helper::write_string(file, name);
        value.write_to_binary(file);
    }

    static data_property read_from_binary(std::istream& file) {
        auto name = helper::read_string(file);
        auto value = data_value<real>::read_from_binary(file);

        return data_property(name, value);
    }
};

/**
     * @brief TFS data column.
     *
     * @tparam real
     */
template <typename real>
class data_vector {
    std::variant<
        std::vector<std::string>,
        std::vector<real>,
        std::vector<int>,
        std::vector<bool>
        > payload;
    DataType type;
    std::string name;

public:
    /**
     * @brief Construct a new data vector object with the given datatype
     *
     * @param t
     * @param s
     */
    data_vector(DataType t, const std::string& s) : type(t), name(s)  {

        switch(t) {
        case DataType::B:
            payload = std::vector<bool>();
            break;
        case DataType::LE:
            payload = std::vector<real>();
            break;
        case DataType::D:
            payload = std::vector<int>();
            break;
        case DataType::S:
            payload = std::vector<std::string>();
            break;
        }
    }

    const std::vector<real>& as_double_vector() const {
        return std::get<std::vector<real>>(payload);
    }
    const std::vector<std::string>& as_string_vector() const {
        return std::get<std::vector<std::string>>(payload);
    }
    const std::vector<bool>& as_bool_vector() const {
        return std::get<std::vector<bool>>(payload);
    }
    const std::vector<int>& as_int_vector() const {
        return std::get<std::vector<int>>(payload);
    }

    std::vector<real>& as_double_vector() {
       return const_cast<std::vector<real>&>(static_cast<const data_vector<real>&>(*this).as_double_vector());
    }
    std::vector<std::string>& as_string_vector() {
       return const_cast<std::vector<std::string>&>(static_cast<const data_vector<real>&>(*this).as_string_vector());
    }
    std::vector<bool>& as_bool_vector() {
       return const_cast<std::vector<bool>&>(static_cast<const data_vector<real>&>(*this).as_bool_vector());
    }
    std::vector<int>& as_int_vector() {
       return const_cast<std::vector<int>&>(static_cast<const data_vector<real>&>(*this).as_int_vector());
    }

    void convert_back(const std::string& s) {
        char* end;
        switch(type) {
        case DataType::D:
            push_back(static_cast<int>(strtol(s.c_str(), &end, 10)));
            break;
        case DataType::LE:
            push_back(strtod(s.c_str(), &end));
            break;
        case DataType::S:
            push_back(s);
            break;
        }
    }

    size_t size() const {
        switch(type) {
        case DataType::D:
            return const_cast<data_vector<real>*>(this)->as_int_vector().size();
            break;
        case DataType::LE:
            return const_cast<data_vector<real>*>(this)->as_double_vector().size();
            break;
        case DataType::S:
            return const_cast<data_vector<real>*>(this)->as_string_vector().size();
            break;
        }
        return 0;
    }

    const std::string& get_name() const {
        return this->name;
    }
    void set_name(const std::string& name) {
        this->name = name;
    }

    void print_at(size_t i, std::ostream& os) const {
        switch(type) {
        case DataType::D:
            os << std::setw(FIELDWIDTH) << const_cast<data_vector<real>*>(this)->as_int_vector()[i] << " ";
            break;
        case DataType::LE:
            os << std::setw(FIELDWIDTH) << const_cast<data_vector<real>*>(this)->as_double_vector()[i] << " ";
            break;
        case DataType::S:
            os << std::setw(FIELDWIDTH) << const_cast<data_vector<real>*>(this)->as_string_vector()[i] << " ";
            break;
        }
    }

    void write_to_binary(std::ostream& file) const {
        helper::write_string(file, name);

        file.write(reinterpret_cast<const char*>(&type), sizeof(DataType));

        size_t count = size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(size_t));

        switch (type) {
        case DataType::LE:
        {
            for (auto& v : as_double_vector())
                file.write(reinterpret_cast<const char*>(&v), sizeof(real));
            break;
        }
        case DataType::S:
            for (auto& s : as_string_vector())
            {
                helper::write_string(file, s);
            }
            break;
        default:
            throw std::runtime_error("not yet implemented");
            break;
        }
    }

    static void read_from_binary(std::istream& file, data_vector& vec) {
        auto name = helper::read_string(file);

        DataType t;
        file.read(reinterpret_cast<char*>(&t), sizeof(DataType));

        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(size_t));

        vec.name = name;
        vec.type = t;

        switch (t) {
        case DataType::LE:
        {
            auto& v = vec.as_double_vector();
            v.reserve(count);
            for (size_t i = 0; i < count; i++) {
                real r;
                file.read(reinterpret_cast<char*>(&r), sizeof(real));
                v.push_back(r);
            }
            break;
        }
        case DataType::S:
        {
            auto& v = vec.as_string_vector();
            v.reserve(count);
            for (size_t i = 0; i < count; i++) {
                auto s = helper::read_string(file);
                v.push_back(s);
            }
            break;
        }
        default:
            throw std::runtime_error("not implemented");
        }

    }

    void reserve(size_t n) {
        switch (type) {
        case DataType::B:
            as_bool_vector().reserve(n);
            break;
        case DataType::D:
            as_int_vector().reserve(n);
            break;
        case DataType::LE:
            as_double_vector().reserve(n);
            break;
        case DataType::S:
            as_string_vector().reserve(n);
            break;
        }
    }

    DataType get_type() const { return type; }

    void push_back(bool b) {
        if (type != DataType::B) throw std::runtime_error("this is not a bool vector");
        as_bool_vector().push_back(b);
    }
    void push_back(int i) {
        if (type != DataType::D) throw std::runtime_error("this is not an int vector");
        as_int_vector().push_back(i);
    }
    void push_back(double d) {
        if (type != DataType::LE) throw std::runtime_error("this is not a double vector");
        as_double_vector().push_back(static_cast<real>(d));
    }
    void push_back(float d) {
        push_back(static_cast<double>(d));
    }
    void push_back(const std::string& s) {
        if (type != DataType::S) throw std::runtime_error("this is not a string vector");
        as_string_vector().push_back(s);
    }
};
inline DataType DT_from_string(const std::string& token) {
    if (token == "%d") return DataType::D;
    if (token == "%le") return DataType::LE;
    if (token == "%b") return DataType::B;
    return DataType::S; // ddefault to S is safe
}
inline const char* string_fromDT(DataType t)
{
    switch (t)
    {
    case DataType::D:
        return "%d";
    case DataType::LE:
        return "%le";
    default:
        return "%s";
    }
}

template <typename real>
std::ostream& operator<<(std::ostream& os, const data_value<real>& v)
{
    return os << v.pretty_print();
}

/**
     * @brief a TFS dataframe. Contains a list of properties and a collection of data columns.
     *
     * @tparam real
     */
template<typename real=double>
class dataframe
{
    std::vector<data_vector<real>> columns;
    std::map<std::string, size_t> column_headers;
    // map would be nice but we  also need to access by index
    //std::map<std::string, data_value<real>> properties;
    std::vector<data_property<real>> properties;
    std::map<std::string, size_t> idx;
    bool ini_complete = false;

public:
    dataframe(){};
    /// <summary>
    ///
    /// </summary>
    /// <param name="path"></param>
    /// <param name="index"></param>
    explicit dataframe(const std::string &path, const std::string& index = "");

    data_vector<real> &get_column(const std::string &name);
    const data_vector<real> &get_column(const std::string &name) const;
    const data_vector<real> &get_column(size_t index) const { return columns.at(index);}

    void reserve_columns(size_t n) { columns.reserve(n); }
    void reserve_rows(size_t n) {
        for (auto& col : columns) {
            col.reserve(n);
        }
    }

    /**
     * @brief Moves a columns into the dataframe
     *
     * @param vec
     * @param name
     */
    void add_column(std::vector<real>&& vec, const std::string& name) {
        column_headers[name] = columns.size();
        data_vector<real> v(DataType::LE, name);
        v.payload.double_vector = std::move(vec);
        columns.push_back(std::move(v));
    }

    void add_column(const data_vector<real>& vec, const std::string& name) {
        column_headers[name] = columns.size();
        columns.push_back(vec);
    }

    /**
     * @brief Adds a double column to the dataframe.
     *
     * @param vec
     * @param name
     */
    void add_column(const std::vector<double>& vec, const std::string& name) {
        column_headers[name] = columns.size();
        data_vector<real> v(DataType::LE, name);
        v.payload.double_vector = vec;
        columns.push_back(std::move(v));
    }

    void add_column(const std::vector<std::string>& vec, const std::string& name) {
        column_headers[name] = columns.size();
        data_vector<real> v(DataType::S, name);
        v.payload.string_vector = vec;
        columns.push_back(std::move(v));
    }

    /**
     * @brief Adds a column of arbitrary type to the dataframe
     *
     * @param name
     * @param t
     * @return data_vector<real>&
     */
    data_vector<real>& add_column(const std::string& name, DataType t) {
        std::cout << "new column " << name << "\n";
        column_headers[name] = columns.size();
        columns.emplace_back(t, name);

        return columns.back();
    }

    //data_value<real>& get_property(const std::string& key) { return properties[key];}
    data_property<real>& get_property(const std::string& key) {
        for (auto& p : properties)
            if (p.name == key) return p;
        throw std::runtime_error("couldn't find key");
        return properties[0];
    }
    data_property<real>& get_property(size_t index) {
        return properties[index];
    }

    void add_property(const std::string& name, const data_value<real> value) {
        properties.push_back(data_property(name, value));
    }

    size_t property_count() const {
        return properties.size();
    }

    std::string pretty_print() const;

    void verify() const;

    size_t size() const
    {
        if (columns.size() == 0)
            return 0;
        return columns[(*column_headers.begin()).second].size();
    }

    size_t column_count() const {
        return columns.size();
    }


    /**
     * @brief Writes the dataframe to a file in tfs format.
     *
     * @param fname
     */
    void to_file(const std::string& fname);

    void to_binary_file(const std::string& fname);
    static dataframe<real> from_binary_file(const std::string& fname);
    void load_from_binary_file(const std::string& fname);

    void write_to_binary(std::ostream& file) const;
    static dataframe<real> read_from_binary(std::istream& stream);
    void load_from_binary(std::istream& stream);


    /**
     * @brief Get the index of the given key
     *
     * @param key
     * @return size_t
     */
    size_t get_index(const std::string& key) { return idx[key]; }

private:
    void read_property(const std::string& line);
    void read_column_headers(const std::string& line);
    void read_column_types(const std::string& line);
    void read_line(const std::string& line);
    void check_ini();
};


template <class ContainerT>
void tokenize(const std::string &str, ContainerT &tokens,
          const std::string &delimiters = " ", bool trimEmpty = false)
{
    std::string::size_type pos, lastPos = str.find_first_not_of(delimiters, 0), length = str.length();

    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (lastPos < length + 1)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos)
        {
            pos = length;
        }

        if (pos != lastPos || !trimEmpty){
            tokens.push_back(
                        value_type(str.data() + lastPos, (size_type)pos - lastPos));
        }

        lastPos = str.find_first_not_of(delimiters, pos);
    }
}

// ---------------------------------------------------------------------------------------------
// - implementation ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

template<typename real>
dataframe<real>::dataframe(const std::string &path, const std::string& index)
{
    std::ifstream file(path);
    std::string line;
    while(std::getline(file, line) && !ini_complete) {
        if (line[0] == '@')
            read_property(line);
        else if (line[0] == '*')
            read_column_headers(line);
        else if (line[0] == '$')
            read_column_types(line);
        check_ini();
    }
    while(std::getline(file, line)) {
        read_line(line);
    }

    if (index.empty()) return;
    auto& index_col = get_column(index).as_string_vector();
    for (int i = 0; i < index_col.size(); i++)
        idx.insert(std::make_pair(index_col[i], i));
}

template<typename real>
data_vector<real>& dataframe<real>::get_column(const std::string& name)
{
    return columns[column_headers[name]];
}

template<typename real>
const data_vector<real>& dataframe<real>::get_column(const std::string& name) const
{
    return columns[column_headers.at(name)];
}

template<typename real>
void dataframe<real>::to_file(const std::string& filename)
{
    std::cout << "writing to file\n";
    std::cout << "this->size() = " << size() << "\n";
    std::ofstream file(filename);

    if (!file.is_open())
        return;

    for (auto& p : properties)
    {
        file << "@ "
                 << std::setw(32) << p.name << " "
                 << std::setw(4) << string_fromDT(p.value.type) << " "
                 << p.value << "\n";
    }
    file << "* ";
    for (auto& kvp : column_headers)
        file << std::setw(FIELDWIDTH) << kvp.first << " ";
    file << "\n$ ";
    for (auto& c : column_headers)
        file << std::setw(FIELDWIDTH) << string_fromDT(columns[c.second].get_type()) << " ";
    file << "\n";

    for (int i = 0; i < size(); i++)
    {
        file << "  ";
        for (auto& c : column_headers)
            columns[c.second].print_at(i, file);
        file << "\n";
    }
    file.close();
}

template<typename real>
void dataframe<real>::read_property(const std::string& line)
{
    std::vector<std::string> tokens;
    tokenize(line, tokens);
    auto t = DT_from_string(tokens[2]);

    switch (t)
    {
    case DataType::D:
    {
        char* pEnd;
        //properties.insert(std::make_pair(tokens[1], data_value<real>((int)strtol(tokens[3].c_str(), &pEnd, 10))));
        properties.push_back(data_property<real>(tokens[1], data_value<real>((int)strtol(tokens[3].c_str(), &pEnd, 10))));
        break;
    }
    case DataType::LE:
    {
        char* pEnd;
        properties.push_back( data_property<real>(tokens[1], data_value<real>(strtod(tokens[3].c_str(), &pEnd))));
        break;
    }

    default:
        // collapse string
        std::ostringstream ss;
        std::copy(tokens.begin() + 3, tokens.end(),
              std::ostream_iterator<std::string>(ss, " "));
        //std::cout << ss.str() << std::endl;
        properties.push_back(data_property<real>(tokens[1], ss.str()));
    }
}

template<typename real>
void dataframe<real>::read_column_headers(const std::string& line)
{
    std::vector<std::string> tokens;
    tokenize(line, tokens);

    for (int i = 0; i < tokens.size() - 1; i++)
        column_headers.insert(std::make_pair(tokens[i + 1], i));
}

template<typename real>
void dataframe<real>::read_column_types(const std::string& line)
{
    std::vector<std::string> tokens;
    tokenize(line, tokens);

    for (auto it = tokens.begin() + 1; it != tokens.end(); ++it)
    {
        columns.emplace_back(DT_from_string(*it), "");
    }
}

template<typename real>
void dataframe<real>::read_line(const std::string& line)
{
    std::vector<std::string> tokens;
    tokenize(line, tokens);

    //std::cout << tokens.size() << " tokens: {" << tokens[0] << ", " << tokens[1] << ", " <<tokens[2] << ", ...}\n";

    for (int i = 0; i < tokens.size(); i++)
    {
        columns[i].convert_back(tokens[i]);
    }
}

template<typename real>
void dataframe<real>::check_ini()
{
    if (columns.size() > 0 && columns.size() == column_headers.size()){
        for (auto kvp: this->column_headers) {
            this->columns[kvp.second].set_name(kvp.first);
        }
        ini_complete = true;
    }
}

template<typename real>
std::string dataframe<real>::pretty_print() const
{
    std::ostringstream ss;
    ss << "DataFrame{\n";
    ss << columns.size() << " columns, " << size() << " rows\n";
    ss << "Headers: \n";
    for (auto& p : properties)
        ss << std::setw(32) << p.name << ": " << p.value << "\n";
    ss << "---\n";
    return ss.str();
}

template<typename real>
void dataframe<real>::verify() const {
    using std::cout;
    using std::endl;
    cout << " -- verifying dataframe --\n";
    for (auto& kvp : column_headers) {
        cout << kvp.first << ": " << columns[kvp.second].size() << " elements \n";
    }
    cout << " per column in columns:\n";
    for(auto& c: columns)
        cout << c.size()  << "elements\n";

    cout << endl;
}
template<typename real>
std::ostream& operator<<(std::ostream& os, const dataframe<real>& df)
{
    return (os << df.pretty_print());
}

template<typename real>
inline void dataframe<real>::write_to_binary(std::ostream& file) const
{
    size_t numprops = properties.size();
    file.write(reinterpret_cast<char*>(&numprops), sizeof(size_t));
    for (auto& p : properties)
        p.write_to_binary(file);
    size_t numcols = columns.size();
    file.write(reinterpret_cast<char*>(&numcols), sizeof(size_t));
    for (auto& c : columns)
        c.write_to_binary(file);
}

template<typename T>
inline dataframe<T> dataframe<T>::read_from_binary(std::istream & file)
{
    dataframe<T> df;
    df.load_from_binary(file);
    return df;
}

template<typename real>
inline void dataframe<real>::load_from_binary(std::istream& stream)
{
    size_t numprops;
    stream.read(reinterpret_cast<char*>(&numprops), sizeof(size_t));

    for (size_t i = 0; i < numprops; i++)
        properties.push_back(data_property<real>::read_from_binary(stream));

    size_t numcolumns;
    stream.read(reinterpret_cast<char*>(&numcolumns), sizeof(size_t));

    columns.reserve(numcolumns);
    for (size_t i = 0; i < numcolumns; i++)
        columns.push_back(data_vector<real>{DataType::LE, ""});

    for (size_t i = 0; i < numcolumns; i++)
    {
        auto& v = columns[i];
        data_vector<real>::read_from_binary(stream, v);
        column_headers.insert(std::make_pair(v.get_name(), i));
    }
}

template<typename real>
inline void dataframe<real>::to_binary_file(const std::string & fname)
{
    std::fstream file(fname, std::ios::binary | std::ios::trunc | std::ios::out);
    write_to_binary(file);
}

template<typename real>
inline dataframe<real> dataframe<real>::from_binary_file(const std::string& fname)
{
    std::fstream file(fname, std::ios::binary | std::ios::in);
    return read_from_binary(file);
}

template<typename real>
inline void dataframe<real>::load_from_binary_file(const std::string& fname)
{
    std::fstream stream(fname, std::ios::binary | std::ios::in);
    load_from_binary(stream);
}
}  // namespace tfs
