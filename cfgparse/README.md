Title:<br/>
<b>cfgparser: Configuration file parser in C, C++ and Java.</b>

Description:<br/>
<p>This is a small package demonstrating how to parse and retrieve structured key-value pairs, with three implementations in C, C++ and Java. Section labels should be enclosed on brackets '[' and ']', multi-value keys should begin with asterisk '*' to flag multiple token parsing, comments are marked with ';' and text is skipped therafter. See '/examples' folder and corresponding output for capabilities.</p>
<ul>
    <li>cfgparse.c (ANSI C): Implementation using only plain C strings library. The main() implementation can be easily transformed into a lightweight string-based parser of key-value pairs with or without section labels. Useful for very small deployments, e.g. with micro-controllers.</li>
	<li>cfgparse.cpp (C++): Implementation in C++ using STL unordered map container as a lookup dictionary.</li>
	<li>ConfigParser.java (Java): Implementation in Java using standard API hashmap container as a lookup dictionary.</li>
</ul>
