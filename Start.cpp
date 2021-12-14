#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>

using std::cout;
using std::endl;
using std::string;

/**
 *		g++ Start.cpp --std=c++17 -o Start && Start
 *
 * You'll see output:
 *
 * 		Hello, Joe
 * 		Goodbye, Again
 *
 * (Along with some other lines).
 *
 * Please see all the comments below.
 */
class SimpleClass {
public:
    /**
     * Let's start simple: std::is_class<T>
     *
     * It's just a template. If the class in question is a class, then it evaluates (at
     * compile time) to true. Otherwise it's false. That part is easy. You could do something
     * like this:
     *
     * 		bool isClass = std::is_class<T>;
     *
     * Cool, right? In fact, let's try that.
     */
    template <typename T>
    void amIAClass(const T & value) const {
        cout << "Is " << value << " << a class: " << std::is_class<T>::value << endl;
    }

    /**
     * This is almost the exact same. I've changed to use the _v variant. You need
     * to understand what's going on.
     *
     * These type_trait templates all produce a struct, and that struct may or may not
     * have a ::type or ::value field. std::is_class<> produces something with a ::value
     * field, and that field is either true or false as apporpriate. So you can
     * either use is_class_v or you can use ::value as we do above. They are the exact
     * same thing, except the _v style is easier to read.
     *
     * Note that if you're on C++11 (yuck), then you don't have the _v forms, although
     * they would be trivial to add yourself.
     */
    template <typename T>
    void amIAClass2(const T & value) const {
        cout << "Is " << value << " << a class: " << std::is_class_v<T> << endl;
    }

    /**
     * Now before we understand how std::enable_if<> works, we need to understand a little
     * about SFINAE.
     *
     * Let's start with the concent of SFINAE -- Substitution Failure Is Not An Error.
     * What this means is if, when expanding the template based on the parameters, if this
     * produces a syntax error, the compiler will attempt to find another template to use
     * instead.
     *
     * So far, so good. We rely on that.
     *
     * So consider this code. It's weird but perfectly valid.
     */
    template <typename T, std::nullptr_t = nullptr>
    void nullExample(const T & value) const {
        cout << "Null Works, " << value << endl;
    }

    /**
     * This is slightly different. I wanted to get rid of the = nullptr part,
     * but to get the compiler to be happy, I had to add typename =. I'm not
     * sure if I prefer one or the other.
     */
    template <typename T, typename = std::nullptr_t >
    void nullExample2(const T & value) const {
        cout << "Null Works, " << value << endl;
    }

    /**
     * std::enable_if takes two arguments, the second being optional (defaults to void).
     * The first should be a boolean expression.
     *
     * In these examples, we're passing either std::is_class_v<T> or !std::is_class_v<T>.
     * Is it a class or is it NOT a class. And that's going to become either true or false.
     *
     * If true, then enable_if creates something like:
     *
     * 		struct { std::nullptr_t type }.
     *
     * So, in effect, this code expands out to:
     *
     * 		template <..., std::nullptr_t = nullptr>.
     *
     * Which we saw above. But if the boolean expression is false, we get:
     *
     * 		template <..., = nullptr>.
     *
     * And that's a syntax error. I'm not sure why the previous one is.
     *
     * When you combine all that, you can have two methods that look nearly the same,
     * and the compiler will attempt to expand them until it finds one that produces sane
     * code. And then that's the one it uses.
     */
    template <typename T, std::enable_if_t< std::is_class_v<T>, std::nullptr_t > = nullptr>
    void hello(const T & name) const {
        cout << "Hello, " << name << endl;
    }

    template <typename T, std::enable_if_t< !std::is_class_v<T>, std::nullptr_t > = nullptr>
    void hello(const T & name) const {
        cout << "Goodbye, " << name << endl;
    }

    /**
     * Now, all that is kind of ugly and still hard to read, so we can make it
     * pretty. Imagine we put these lines into an include file.
     */
    template <typename T>
        using IsClass = std::enable_if_t< std::is_class_v<T>, std::nullptr_t >;

    template <typename T>
        using IsNotClass = std::enable_if_t< !std::is_class_v<T>, std::nullptr_t >;

    /**
     * And then we do these two.
     */
    template <typename T, IsClass<T> = nullptr >
    void cleaner(const T & name) const {
        cout << "Is Class, " << name << endl;
    }

    template <typename T, IsNotClass<T> = nullptr >
    void cleaner(const T & name) const {
        cout << "Not Class, " << name << endl;
    }

    /**
     * Here's another way to accomplish the same thing. In this case,
     * we rely on the macro to return either "void" or "", and in the
     * latter case, we end up trying to build a method with no return type,
     * so it can't use that one and uses the other version.
     */

    template <typename T>
        using IsClassV = std::enable_if_t< std::is_class_v<T>, void >;
    template <typename T>
        using IsNotClassV = std::enable_if_t< !std::is_class_v<T>, void >;

    template <typename T>
    IsClassV<T>
    another(const T & name) const {
        cout << "Is Class, " << name << endl;
    }

    template <typename T>
    IsNotClassV<T>
    another(const T & name) const {
        cout << "Is not Class, " << name << endl;
    }

    /**
     * I still find this a little confusing, so I'm going to back away
     * a little and rewrite it. What is happening here? Well...
     * We're making a second argument to the method with a type of std::nullptr_t
     * and a default value of nullptr. Below we'll actually invoke it that way.
     *
     * Remember, methods can take arguments without names. You might see thise
     * most commonly when you have defined a method but you're not fully implemented,
     * and so you don't give it a name. Or see my main() below where I put in
     * placeholders for argc and argv, but I didn't name them.
     *
     * Well, this style does exactly the same thing.
     */
    template <typename T>
    void asArgument(const T & name, IsClass<T> = nullptr) const {
        cout << "AsArg: Is Class, " << name << endl;
    }

    template <typename T>
    void asArgument(const T & name, IsNotClass<T> = nullptr) const {
        cout << "AsArg: Is Not Class, " << name << endl;
    }
};


int main(int, char **)
{
    cout << std::boolalpha;
    SimpleClass sc;

    sc.amIAClass( string{"A String"} );
    sc.amIAClass( "Constant" );
    sc.amIAClass2( "Constant" );

    sc.nullExample( "Some Null" );
    sc.nullExample2( "Again" );

    sc.hello( string{"Joe"} );
    sc.hello( "Again" );

    sc.cleaner( string{"Joe"} );
    sc.cleaner( "Again" );

    sc.another( string{"Joe"} );
    sc.another( "Again" );

    sc.asArgument( string{"Joe"}, nullptr);
    sc.asArgument( "Again" );

    return 0;
}
