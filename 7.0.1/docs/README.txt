/* -*-C-*- */
/** @cond API */

/**
 * @mainpage The Funambol C++ SyncML Client Library.
 *
 * Welcome to the Funambol C++ SyncML Client Library! As the name implies,
 * this library provides an implementation of the SyncML protocol. It can
 * be compiled for Windows, Linux, Mac OS X and probably many other POSIX
 * compliant systems.
 *
 * The module list in this documentation describes everything a
 * developer needs to know to implement a SyncML client - or so we
 * hope...
 *
 * @todo A short HOWTO probably would be good.
 *
 * If some information is missing or not explained well,
 * please do not hesitate to ask for clarifications. A good starting
 * point for such inquiries is the Funambol developers mailing list:
 * https://lists.sourceforge.net/lists/listinfo/funambol-developers
 *
 * For developers working on the core library it will also be possible
 * to generate an extended version of this documentation, but this is
 * not supported yet.
 *
 * @cond DEV
 * More information for core developers:
 * @subpage Doxygen
 * @endcond
 */

/** @endcond */

/** @cond DEV */

/**
 * @page Doxygen Generating documentation with Doxygen 
 *
 * To use the Doxygen config provided in the "doc" directory for the Funambol C++ client API,
 * invoke "doxygen <full path>/Doxyfile" in the directory where doxygen is supposed to
 * generate the documentation. For this to work it must be
 * told where to find sources plus various other options, by setting the
 * following environment variables:
 * - FUNAMBOL_NATIVE_ROOT = <root directory of the C++ client library>
 * - ENABLED_SECTIONS = "API" or "API DEV" to choose between user documentation and full documentation
 * - HAVE_DOT = YES|NO to configure whether Doxygen may use the "dot" tool from the Graphviz package
 *              to generate class graphs
 *
 * Here are guidelines for writing good Doxygen comments that are consistent with the
 * other Doxygen comments in the C++ client library:
 * - To be consistent with Java, use @ instead of \ to introduce special commands.
 * - JAVADOC_AUTOBRIEF is active, so that the first "sentence" of a
 *   comment automatically becomes the brief documentation for an
 *   entity
 * - Use C-style comment blocks:
@verbatim
     /**
      * Brief comment. More comments...
      */
     void foo();
                
     class bar {
        int member;    /**< member comment */
     };
@endverbatim
 * - Use \@cond DEV \@endcond to mark content relevant only for core developers and
 *   the same with API for content that is to be visible to users of the library.
 * - Use groups to keep related classes together and add comments about general concepts
 *   to these groups; README.txt files can be used for that purpose.
 * - All comments belong into the header files, unless the entity is only defined in a
 *   source file.
 * - Comments should never be duplicated.
 * - When implementing a derived class, do not copy the comments of the virtual methods
 *   you implement unless you want to modify them: Doxygen will automatically use the
 *   comments of the base class in all derived classes if those classes do not provide
 *   their own comments.
 * - For class properties and their get/set methods one
 *   comment is enough if there is nothing special to be said for
 *   aach of them. Document the "get()" method because "returns <description
 *   of the property>" is easy to read and often fits into one line. Doxygen
 *   will show the other entities without description, so it is clear that
 *   they exist.
 * - In case it isn't obvious, "<description of the property>" should
 *   add information and not just repeat the name of the property.
 *   This is a general rule for all comments; if there isn't anything
 *   additional to say, then at least paraphrase what the function
 *   name already expresses. It might help to clarify what was meant.
 * - The Doxygen configuration was changed so that parameters do not
 *   have to be documented. However, if one parameter is documented
 *   and some other isn't, then Doxygen will still warn about it and
 *   all parameters should be documented.
 *
 * Some comments about coding style which are relevant here because they
 * will show up as artifacts in the API documentation:
 * - The header file of a class should only contain entities which
 *   really are part of the public interface of the class. Doxygen
 *   will hide private members of a class automatically, but
 *   everything else shows up.
 * - Global #defines for constants are problematic for various
 *   reasons, among them that they litter the global namespace. This
 *   is bad for the compiler, but also for Doxygen because it cannot
 *   attach them to the entity that needs them. Better use static const
 *   members of classes: those where the value can be defined in the class
 *   itself (basic types) are displayed with value in the documentation, but
 *   for string constants this does not work. Also, one needs both a "static
 *   const char []" member declaration in the class as well as a
 *   definition with the content elsewhere.
 * - Define properties as "private" and write get/set methods for them. That
 *   is a cleaner both from a C++ design perspective and nicer
 *   in the Doxygen documentation because private members are not shown
 *   in the API documentation.
 * - If you find yourself in the situation where you do a lot of
 *   cut-and-paste to move comments from one function to others,
 *   consider what the common functionality is and whether the
 *   functions can be replaced by one with one or more additional
 *   parameters.
 */

/** @endcond */

