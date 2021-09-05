#include "json.h"

#include <cmath>

using namespace std;

namespace Json {

    Document::Document(Node root) : root(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root;
    }

    Node LoadNode(istream& input);

    Node LoadArray(istream& input) {
        vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadBool(istream& input)
    {
        char c;
        input >> c;

        if (c == 't' || c == 'T')
        {
            input.ignore(3);
            return Node(bool(true));
        }
        else if (c == 'f' || c == 'F')
        {
            input.ignore(4);
            return Node(bool(false));
        }

        throw invalid_argument("bad bool");
    }

    Node LoadNumber(istream& input)
    {
        double num;
        input >> num;
        return Node(num);
    }

    Node LoadString(istream& input) {
        string line;
        getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(istream& input) {
        map<string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(istream& input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        }
        else if (c == '{') {
            return LoadDict(input);
        }
        else if (c == '"') {
            return LoadString(input);
        }
        else if (c == 't' || c == 'f')
        {
            input.putback(c);
            return LoadBool(input);
        }
        else if ((c >= '0' && c <= '9') || c == '-')
        {
            input.putback(c);
            return LoadNumber(input);
        }
        else
        {
            throw invalid_argument("bad load node");

        }
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }
}


std::ostream& operator<<(std::ostream& out, const Json::Node& n)
{
    using namespace std;
    using namespace Json;

    if (n.Is<vector<Node>>())
    {
        const auto& narr = n.AsArray();

        if (narr.empty())
        {
            out << "[]";
            return out;
        }

        out << "[" << narr[0];

        for (size_t i = 1; i < narr.size(); ++i)
        {
            out << ", " << narr[i];
        }

        out << "]";
        return out;
    }

    if (n.Is<map<string, Node>>())
    {
        const auto& nmap = n.AsMap();

        if (nmap.empty())
        {
            out << "{}";
            return out;
        }

        out << "{\"" << nmap.begin()->first << "\": " << nmap.begin()->second;

        for (auto it = next(nmap.begin()); it != nmap.end(); ++it)
        {
            out << ", \"" << it->first << "\": " << it->second;
        }

        out << "}";
        return out;
    }

    if (n.Is<double>())
    {
        auto d = n.AsDouble();
        double intpart;

        if (std::modf(d, &intpart) == 0)
        {
            out << n.AsInt();
        }
        else
        {
            out << n.AsDouble();
        }

        return out;
    }

    if (n.Is<bool>())
    {
        out << (n.AsBoolean() ? "true" : "false");
        return out;
    }

    if (n.Is<string>())
    {
        out << "\"" << n.AsString() << "\"";
        return out;
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const Json::Document& d)
{
    out << d.GetRoot();
    return out;
}