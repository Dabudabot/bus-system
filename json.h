#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iomanip>

namespace Json {

    class Node : std::variant<std::vector<Node>,
        std::map<std::string, Node>,
        double,
        bool,
        std::string> {
    public:
        using variant::variant;

        template<class T>
        bool Is() const
        {
            return std::holds_alternative<T>(*this);
        }

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        long AsInt() const {
            return static_cast<long>(std::get<double>(*this));
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }
        const double& AsDouble() const
        {
            return std::get<double>(*this);
        }
        const auto& AsBoolean() const
        {
            return std::get<bool>(*this);
        }
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root;
    };

    Document Load(std::istream& input);
}

std::ostream& operator<<(std::ostream& out, const Json::Node& n);
std::ostream& operator<<(std::ostream& out, const Json::Document& d);