/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Kevin Waldock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once
#ifndef CSVReader_H
#define CSVReader_H

#include <iostream>
#include <fstream>
#include <string>
#include <type_traits>
#include <sstream>
#include <array>
#include <exception>
#include <stdexcept>
#include <utility>

#include "invoke_default.hpp"

namespace CSVReader
{
    // ========= Exceptions START ===========
    /*!
     * \brief Exception for parsing errors.
     */
    class parse_error : public std::logic_error
    {
    private:
        int _line;
        int _field;
    public:
        explicit parse_error(const char *msg, int line, int field) : std::logic_error(msg), _line(line), _field(field) {}
        explicit parse_error(std::string msg, int line, int field) : parse_error(msg.c_str(), line, field) {}

        inline int get_line_number() const
        {
            return _line;
        }
        inline int get_field_number() const
        {
            return _field;
        }
    };
    // ========= Exceptions END ===========



    // ========= Utils START ===========
    // This is a feature built-in for C++14
    namespace detail
    {
        template <std::size_t... I>
        class index_sequence {};

        template <std::size_t N, std::size_t ...I>
        struct make_index_sequence : make_index_sequence < N - 1, N - 1, I... > {};

        template <std::size_t ...I>
        struct make_index_sequence<0, I...> : index_sequence<I...> {};
    }

    // This is for the CSVBatchReader
    // This template class depends on .push_back()
    template<class ContainerValueT, class ContainerT>
    struct CommonContainerUtils
    {
        static void Add(ContainerT *container, const ContainerValueT &value)
        {
            container->push_back(value);
        }
    };

    namespace detail
    {

        template<class StrT,
                 class CharT,
                 class StrTUtils,
                 class Converter>
        class CSVReaderBase
        {
        protected:
            size_t _currentCharIndex;
            CharT _sep;
            StrT _currentLine; // Will be written by the derived class
            int _fieldTracker; // Will be written by the derived class
            int _lineTracker;  // Will be written by the derived class

            CSVReaderBase(CharT sep) : _currentCharIndex(0u), _sep(sep),
                _currentLine(""), _fieldTracker(0), _lineTracker(0) {}

            inline StrT NextField()
            {
                size_t newCharIndex = _currentCharIndex;
                if(!StrTUtils::find(_currentLine, _sep, newCharIndex))
                    newCharIndex = StrTUtils::length(_currentLine);

                StrT next = StrTUtils::substring(_currentLine, _currentCharIndex, newCharIndex - _currentCharIndex);
                _currentCharIndex = newCharIndex + 1;
                return next;
            }

            inline void SkipField()
            {
                size_t newCharIndex = _currentCharIndex;
                if(!StrTUtils::find(_currentLine, _sep, newCharIndex))
                    newCharIndex = StrTUtils::length(_currentLine);
                _currentCharIndex = newCharIndex + 1;
            }

            inline bool HasNext()
            {
                return _currentCharIndex <= StrTUtils::length(_currentLine);
            }

            template<typename ToType>
            inline void SafeConvert(ToType *to, const StrT &from)
            {
                try
                {
                    Converter::Convert(to, from);
                }
                catch(...)
                {
                    ThrowParseErrorInCatchContext();
                }
            }

            inline void ThrowParseErrorInCatchContext()
            {
                std::throw_with_nested(parse_error(std::string("Failed to parse field ") + std::to_string(_fieldTracker) + " at line " + std::to_string(_lineTracker), _lineTracker, _fieldTracker));
            }
        };
    }
    // ========= Utils END ===========



    // ========= Readers START ===========
    template<class StrElementType, class StrElementTraits, class StrElementAlloc = std::allocator<StrElementType> >
    struct IfStreamReader
    {
        using target_ifstream = std::basic_ifstream<StrElementType, StrElementTraits>;
        using target_string = std::basic_string<StrElementType, StrElementTraits, StrElementAlloc>;
        typedef target_string string_type;

        target_ifstream *_reader;
        IfStreamReader(target_ifstream *reader) : _reader(reader) {}

        target_string read_line()
        {
            target_string str;
            std::getline(*_reader, str);
            return str;
        }
    };
    template<class StrElementType, class StrElementTraits>
    constexpr IfStreamReader<StrElementType, StrElementTraits> MakeIfStreamReader(std::basic_ifstream<StrElementType, StrElementTraits> *reader)
    {
        return IfStreamReader<StrElementType, StrElementTraits>(reader);
    }

    template<class StrElementType, class StrElementTraits, class StrElementAlloc>
    struct StringReader
    {
        using target_istringstream = std::basic_istringstream<StrElementType, StrElementTraits, StrElementAlloc>;
        using target_string = std::basic_string<StrElementType, StrElementTraits, StrElementAlloc>;
        typedef target_string string_type;

        target_istringstream _txt;
        StringReader(target_string str) : _txt(str) {}

        target_string read_line()
        {
            target_string str;
            std::getline(_txt, str);
            return str;
        }
    };
    template<class StrElementType, class StrElementTraits, class StrElementAlloc>
    constexpr StringReader<StrElementType, StrElementTraits, StrElementAlloc> MakeStringReader(const std::basic_string<StrElementType, StrElementTraits, StrElementAlloc> &str)
    {
        return StringReader<StrElementType, StrElementTraits, StrElementAlloc>(str);
    }

    template<class StrT>
    struct DirectReader
    {
        typedef StrT string_type;

        DirectReader(const StrT &data) : _data(data) {}
        StrT read_line()
        {
            return _data;
        }
    private:
        StrT _data;
    };
    template<class StrT>
    constexpr DirectReader<StrT> MakeDirectReader(const StrT &data)
    {
        return DirectReader<StrT>(data);
    }

    // ========= Readers END ===========







    // ========= Special Attributes START ===========
    // 1. Alternative Parameter - CSVDiscard
    /*!
     * \brief Special CSVReader parameter value for ignoring a field.
     */
    struct CSVDiscard {};

    // 2. Alternative Parameter - CSVValidator
    /*!
     * \brief Special CSVReader parameter value for validating a field.
     * \see MakeCSVValidator()
     */
    template<typename T, typename ValidatorFunc>
    struct CSVValidator
    {
        typedef T value_type;
        CSVValidator(T *value, const ValidatorFunc &validatorFunction) : _value(value), _validatorFunction(validatorFunction) {}

        bool Validate() const
        {
            return idef::invoke_or_noop<bool>(idef::default_if_nullptr(_validatorFunction, [](T *)
            {
                return true;
            }), _value);
        }
        T *Get()
        {
            return _value;
        }
    private:
        T *_value;
        ValidatorFunc _validatorFunction;
    };
    /*!
     * \brief Creates a new CSVValidator.
     * \see CSVValidator
     */
    template<typename T, typename ValidatorFunc>
    constexpr CSVValidator<T, ValidatorFunc> MakeCSVValidator(T *value, ValidatorFunc validatorFunction)
    {
        return CSVValidator<T, ValidatorFunc>(value, validatorFunction);
    }

    // 3. Alternative Parameter - CSVPostProcessor
    /*!
     * \brief Special CSVReader parameter value for post processing a field.
     * \see MakeCSVPostProcessor()
     *
     * The post processor will be called before the value will be writting into the pointer.
     */
    template<typename T, typename PostProcessorFunc, typename ValidatorFunc>
    struct CSVPostProcessor
    {
        typedef T value_type;

        CSVPostProcessor(T *value, const PostProcessorFunc &postProcessorFunction) : CSVPostProcessor(value, postProcessorFunction, nullptr) {}
        CSVPostProcessor(T *value, const PostProcessorFunc &postProcessorFunction, const ValidatorFunc &validatorFunction)
            : _value(value), _validatorFunction(validatorFunction), _postProcessorFunction(postProcessorFunction) {}

        bool Validate() const
        {
            return idef::invoke_or_noop<bool>(idef::default_if_nullptr(_validatorFunction, [](T *)
            {
                return true;
            }), _value);
        }
        inline void PostProcess()
        {
            idef::invoke_or_noop<void>(_postProcessorFunction, *_value);
        }
        T *Get()
        {
            return _value;
        }
    private:
        T *_value;
        ValidatorFunc _validatorFunction;
        PostProcessorFunc _postProcessorFunction;
    };
    /*!
     * \brief Creates a new CSVPostProcessor.
     * \see CSVPostProcessor
     */
    template<typename T, typename PostProcessorFunc>
    constexpr CSVPostProcessor<T, PostProcessorFunc, std::nullptr_t> MakeCSVPostProcessor(T *value, PostProcessorFunc postProcessorFunction)
    {
        return CSVPostProcessor<T, PostProcessorFunc, std::nullptr_t>(value, postProcessorFunction);
    }
    /*!
     * \brief Creates a new CSVPostProcessor.
     * \see CSVPostProcessor
     */
    template<typename T, typename PostProcessorFunc, typename ValidatorFunc>
    constexpr CSVPostProcessor<T, PostProcessorFunc, ValidatorFunc> MakeCSVPostProcessor(T *value, PostProcessorFunc postProcessorFunction, ValidatorFunc validatorFunction)
    {
        return CSVPostProcessor<T, PostProcessorFunc, ValidatorFunc>(value, postProcessorFunction, validatorFunction);
    }



    // 4. Alternative parameter - CSVOptional
    /*!
     * \brief Special CSVReader parameter value for marking a field as optional.
     * \see MakeCSVOptional()
     *
     * If the reader cannot read further, then the default value is used
     * instead of throwing an exception.
     */
    template<typename T, typename ValidatorFunc, typename PostProcessorFunc>
    struct CSVOptional
    {
        typedef T value_type;

        CSVOptional(T* value, const T defVal, bool shouldAssignDefaultOnEmpty) : CSVOptional(value, defVal, shouldAssignDefaultOnEmpty, nullptr) {}
        CSVOptional(T* value, const T defVal, bool shouldAssignDefaultOnEmpty, const ValidatorFunc& validatorFunction) :
            CSVOptional(value, defVal, shouldAssignDefaultOnEmpty, validatorFunction, nullptr) {}
        CSVOptional(T* value, const T defVal, bool shouldAssignDefaultOnEmpty, const ValidatorFunc& validatorFunction, const PostProcessorFunc& postProcessorFunction) :
            _value(value), _defaultValue(defVal), _shouldAssignDefaultOnEmpty(shouldAssignDefaultOnEmpty),
            _validatorFunction(validatorFunction), _postProcessorFunction(postProcessorFunction) {}

        bool Validate() const
        {
            return idef::invoke_or_noop<bool>(idef::default_if_nullptr(_validatorFunction, [](T *)
            {
                return true;
            }), _value);
        }
        inline void PostProcess()
        {
            idef::invoke_or_noop<void>(_postProcessorFunction, *_value);
        }
        inline void AssignDefault()
        {
            *_value = _defaultValue;
        }
        inline T *Get()
        {
            return _value;
        }
        inline bool ShouldAssingDefaultOnEmpty() const {
            return _shouldAssignDefaultOnEmpty;
        }
    private:
        T *_value;
        T _defaultValue;
        bool _shouldAssignDefaultOnEmpty;
        ValidatorFunc _validatorFunction;
        PostProcessorFunc _postProcessorFunction;
    };
    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT>
    constexpr CSVOptional<T, std::nullptr_t, std::nullptr_t> MakeCSVOptional(T* value, OT defVal) {
        return CSVOptional<T, std::nullptr_t, std::nullptr_t>(value, defVal, false, nullptr, nullptr);
    }
    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT, typename ValidatorFunc>
    constexpr CSVOptional<T, ValidatorFunc, std::nullptr_t> MakeCSVOptional(T* value, OT defVal, ValidatorFunc validatorFunction) {
        return CSVOptional<T, ValidatorFunc, std::nullptr_t>(value, defVal, false, validatorFunction, nullptr);
    }
    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT, typename ValidatorFunc, typename PostProcessorFunc>
    constexpr CSVOptional<T, ValidatorFunc, PostProcessorFunc> MakeCSVOptional(T* value, OT defVal, ValidatorFunc validatorFunction, PostProcessorFunc postProcessorFunction) {
        return CSVOptional<T, ValidatorFunc, PostProcessorFunc>(value, defVal, false, validatorFunction, postProcessorFunction);
    }

    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT>
    constexpr CSVOptional<T, std::nullptr_t, std::nullptr_t> MakeCSVOptionalEmpty(T* value, OT defVal) {
        return CSVOptional<T, std::nullptr_t, std::nullptr_t>(value, defVal, true, nullptr, nullptr);
    }
    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT, typename ValidatorFunc>
    constexpr CSVOptional<T, ValidatorFunc, std::nullptr_t> MakeCSVOptionalEmpty(T* value, OT defVal, ValidatorFunc validatorFunction) {
        return CSVOptional<T, ValidatorFunc, std::nullptr_t>(value, defVal, true, validatorFunction, nullptr);
    }
    /*!
     * \brief Creates a new CSVOptional.
     * \see CSVOptional
     */
    template<typename T, typename OT, typename ValidatorFunc, typename PostProcessorFunc>
    constexpr CSVOptional<T, ValidatorFunc, PostProcessorFunc> MakeCSVOptionalEmpty(T* value, OT defVal, ValidatorFunc validatorFunction, PostProcessorFunc postProcessorFunction) {
        return CSVOptional<T, ValidatorFunc, PostProcessorFunc>(value, defVal, true, validatorFunction, postProcessorFunction);
    }

    // 5. Reader in Reader - CSVOptional
    /*!
     * \brief Special CSVReader parameter value for creating a sub-reader.
     * \see MakeCSVSubReader()
     * This class can read fields, which are seperated again with another token.
     */
    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class... RestValues>
    struct CSVSubReader;

    // 6. Reading in container
    /*!
     * \brief Special CSVReader parameter value for reading a collection of literal types.
     * \see MakeCSVBatchReader
     */
    template<class ContainerValueT,
             class Container,
             class ContainerUtils,
             class StrT,
             class CharT,
             class StrTUtils,
             class Converter,
             class PostProcessorFunc>
    struct CSVBatchReader : detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>
    {
    private:
        Container* _container;
        PostProcessorFunc _postProcessorFunction;
    public:

        CSVBatchReader(CharT sep, Container* container, const PostProcessorFunc &postProcessorFunction) :
            detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>(sep), _container(container), _postProcessorFunction(postProcessorFunction) {}

        inline void ReadDataLine(const StrT &val)
        {
            this->_currentLine = val;
            while(this->HasNext())
            {
                StrT from = this->NextField();
                if(from == "")
                    continue;
                ContainerValueT to;
                this->SafeConvert(&to, from);
                idef::invoke_or_noop<void>(_postProcessorFunction, to);
                ContainerUtils::Add(_container, to);
            }
        }
    };

    // 7. Reading with iteration
    /*!
     * \brief Special CSVReader parameter value for iterating through an unknown number of types.
     * \see MakeCSVIterator()
     */
    template<class StrT,
             class CharT,
             class StrTUtils,
             class Converter,
             class IteratorFunc>
    struct CSVIterator : detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>
    {
    private:
        bool _isOptional;
        IteratorFunc _iteratorFunc;
    public:
        CSVIterator(CharT sep, bool isOptional, const IteratorFunc &iteratorFunc) :
            detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>(sep), _isOptional(isOptional), _iteratorFunc(iteratorFunc) {}

        inline void ReadDataLine(const StrT &val)
        {
            this->_currentLine = val;
            while(this->HasNext())
            {
                StrT next = this->NextField();
                if(!(next == ""))
                    _iteratorFunc(next);
            }
        }

        bool IsOptional() const
        {
            return _isOptional;
        }
    };


    // ========= Special Attributes END ===========


    /*
    * The Default CSV Converter uses the STL library to do the most of the conversion.
    */
    /*!
     * \brief The default converter to convert strings to literal types.
     */
    template<class StrType>
    struct DefaultCSVConverter
    {
        static void Convert(double *out, const StrType &field)
        {
            *out = std::stod(field);
        }
        static void Convert(float *out, const StrType &field)
        {
            *out = std::stof(field);
        }
        static void Convert(int *out, const StrType &field)
        {
            *out = std::stoi(field);
        }
        static void Convert(long *out, const StrType &field)
        {
            *out = std::stol(field);
        }
        static void Convert(long long *out, const StrType &field)
        {
            *out = std::stoll(field);
        }
        static void Convert(long double *out, const StrType &field)
        {
            *out = std::stold(field);
        }
        static void Convert(unsigned int *out, const StrType &field)
        {
            *out = static_cast<unsigned int>(std::stoul(field));
        }
        static void Convert(unsigned long *out, const StrType &field)
        {
            *out = std::stoul(field);
        }
        static void Convert(unsigned long long *out, const StrType &field)
        {
            *out = std::stoull(field);
        }
        static void Convert(bool *out, const StrType &field)
        {
            if(field == "0" || field == "") // FIXME: Is it correct? Or too hackish?
                *out = false;
            else if(field == "!0" || field == "1") // FIXME: Is it correct? Or too hackish?
                *out = true;
            else
                throw std::invalid_argument(std::string("Could not convert to bool (must be empty, \"0\", \"!0\" or \"1\"), got \"") + field + std::string("\""));
        }
        static void Convert(StrType *out, const StrType &field)
        {
            *out = field;
        }
    };


    /*!
     * \brief The default wrapper for strings (Works for STL strings)
     * \see MakeCSVReaderFromBasicString()
     */
    template<class StrElementType, class StrElementTraits, class StrElementAlloc>
    struct DefaultStringWrapper
    {
        using target_string = std::basic_string<StrElementType, StrElementTraits, StrElementAlloc>;

        static bool find(const target_string &str, StrElementType sep, size_t &findIndex)
        {
            size_t pos = str.find(sep, findIndex);
            if(pos == target_string::npos)
                return false;
            findIndex = pos;
            return true;
        }

        static size_t length(const target_string &str)
        {
            return str.length();
        }

        static target_string substring(const target_string &str, size_t pos, size_t count)
        {
            return str.substr(pos, count);
        }
    };





    /*!
     * \brief Provides features to read out CSV data.
     * \see MakeCSVReader()
     *
     * This class provides features to read out CSV data and converting them directly to literal types.
     *
     * Use MakeCSVReader or MakeCSVReaderFromBasicString for easy construction of this class.
     *
     */
    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter>
    class CSVReader : detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>
    {
    private:
        Reader *_reader;
        int _currentTotalFields;
        bool _requireReadLine;
    public:
        CSVReader(Reader *reader, CharT sep) : detail::CSVReaderBase<StrT, CharT, StrTUtils, Converter>(sep), _reader(reader),
            _currentTotalFields(0), _requireReadLine(true) {}
        CSVReader(const CSVReader &other) = delete;
        CSVReader(CSVReader &&other) = default;
        ~CSVReader() = default;

    private:
        inline void ThrowIfOutOfBounds()
        {
            if(this->_currentCharIndex > StrTUtils::length(this->_currentLine))
                throw parse_error("Expected " + std::to_string(this->_currentTotalFields) + " CSV-Fields, got "
                                  + std::to_string(this->_fieldTracker) + " at line "
                                  + std::to_string(this->_lineTracker) + "!", this->_lineTracker, this->_fieldTracker);
        }
        template<class T, class... RestValues>
        void ReadNext(T nextVal, RestValues &&... restVals)
        {
            static_assert(std::is_pointer<T>::value, "All values which are unpacked must be pointers (except CSVDiscard, CSVVaildate, CSVDiscard, CSVOptional, CSVSubReader)!");
            ThrowIfOutOfBounds();

            //Here do conversion code
            this->SafeConvert(nextVal, this->NextField());

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class... RestValues>
        void ReadNext(CSVDiscard, RestValues &&... restVals)
        {
            this->_fieldTracker++;
            this->SkipField();
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class ValidateT, class ValidatorFunc, class... RestValues>
        void ReadNext(CSVValidator<ValidateT, ValidatorFunc> nextVal, RestValues &&... restVals)
        {
            ThrowIfOutOfBounds();

            this->SafeConvert(nextVal.Get(), this->NextField());

            if(!nextVal.Validate())
                throw std::logic_error("Validation failed at field " + std::to_string(this->_fieldTracker) + " at line " + std::to_string(this->_lineTracker) + "!");

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class PostProcessorT, class PostProcessorFunc, class ValidatorFunc, class... RestValues>
        void ReadNext(CSVPostProcessor<PostProcessorT, PostProcessorFunc, ValidatorFunc> nextVal, RestValues &&... restVals)
        {
            ThrowIfOutOfBounds();

            this->SafeConvert(nextVal.Get(), this->NextField());
            if(!nextVal.Validate())
                throw std::logic_error("Validation failed at field " + std::to_string(this->_fieldTracker) + " at line " + std::to_string(this->_lineTracker) + "!");
            nextVal.PostProcess();

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class OptionalT, class ValidatorFunc, class PostProcessorFunc, class... RestValues>
        void ReadNext(CSVOptional<OptionalT, ValidatorFunc, PostProcessorFunc> optionalObj, RestValues &&... restVals)
        {
            // If we already reached the end, then assign default
            if(this->_currentCharIndex >= StrTUtils::length(this->_currentLine))
                optionalObj.AssignDefault();
            else
            {
                StrT nextField = this->NextField();
                if(!optionalObj.ShouldAssingDefaultOnEmpty() || StrTUtils::length(nextField) > 0) {
                    this->SafeConvert(optionalObj.Get(), nextField);
                    if (!optionalObj.Validate())
                        throw std::logic_error("Validation failed at field " + std::to_string(this->_fieldTracker) + " at line " + std::to_string(this->_lineTracker) + "!");
                    optionalObj.PostProcess();
                } else {
                    optionalObj.AssignDefault();
                }
            }

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class SubReader, class SubStrT, class SubCharT, class SubStrTUtils, class SubConverter, class... SubValues, class... RestValues>
        void ReadNext(CSVSubReader<SubReader, SubStrT, SubCharT, SubStrTUtils, SubConverter, SubValues...> subReaderObj, RestValues &&... restVals)
        {
            if(!subReaderObj.IsOptional())
                ThrowIfOutOfBounds();

            // We don't have to check for subReaderObj.IsOptional again, because
            // ThrowIfOutOfBounds() would have thrown already
            if(!(this->_currentCharIndex >= StrTUtils::length(this->_currentLine)))
            {
                try
                {
                    subReaderObj.ReadDataLine(this->NextField());
                }
                catch(...)
                {
                    this->ThrowParseErrorInCatchContext();
                }
            }

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class ReaderContainerValueT, class ReaderContainer, class ReaderContainerUtils,
                 class ReaderStrT, class ReaderCharT, class ReaderStrTUtils, class ReaderConverter, class PostProcessorFunc, class... RestValues>
        void ReadNext(CSVBatchReader<ReaderContainerValueT, ReaderContainer, ReaderContainerUtils, ReaderStrT, ReaderCharT, ReaderStrTUtils, ReaderConverter, PostProcessorFunc> subBatchReaderObj, RestValues &&... restVals)
        {
            ThrowIfOutOfBounds();

            try
            {
                subBatchReaderObj.ReadDataLine(this->NextField());
            }
            catch(...)
            {
                this->ThrowParseErrorInCatchContext();
            }

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        template<class IterStrT, class IterCharT, class IterStrTUtils, class IterConverter, class IteratorFunc, class... RestValues>
        void ReadNext(CSVIterator<IterStrT, IterCharT, IterStrTUtils, IterConverter, IteratorFunc> iteratorObj, RestValues &&... restVals)
        {
            if(!iteratorObj.IsOptional())
                ThrowIfOutOfBounds();

            // We don't have to check for iteratorObj.IsOptional again, because
            // ThrowIfOutOfBounds() would have thrown already
            if(!(this->_currentCharIndex >= StrTUtils::length(this->_currentLine)))
            {
                try
                {
                    iteratorObj.ReadDataLine(this->NextField());
                }
                catch(...)
                {
                    this->ThrowParseErrorInCatchContext();
                }
            }

            this->_fieldTracker++;
            ReadNext(std::forward<RestValues>(restVals)...);
        }

        void ReadNext() {}
    public:
        /*!
         * \brief Read the next data line and pushes the result directly to the parameter.
         *
         * allValues must be pointer with the exception of:
         *      * CSVDiscard
         *      * CSVValidator
         *      * CSVPostProcessor
         *      * CSVOptional
         *      * CSVSubReader
         *      * CSVBatchReader
         *      * CSVIterator
         *
         * \throws std::nested_exception When a parsing or conversion error happens.
         *
         */
        template<typename... Values>
        CSVReader &ReadDataLine(Values &&... allValues)
        {
            this->_lineTracker++;
            this->_currentCharIndex = 0;
            _currentTotalFields = sizeof...(allValues);
            this->_fieldTracker = 0; // We need the tracker at 0 (because of out of range exception)
            if(_requireReadLine)
                this->_currentLine = _reader->read_line();
            ReadNext(std::forward<Values>(allValues)...);
            _requireReadLine = true;


            return *this;
        }


        /*!
         * \brief Read the next data line and calls iteration function with passing every field.
         *
         * \throws std::nested_exception When a parsing or conversion error happens.
         */
        template<class IteratorFunc>
        CSVReader& IterateDataLine(const IteratorFunc &iteratorFunc)
        {
            this->_lineTracker++;
            this->_currentCharIndex = 0;
            this->_fieldTracker = 0; // We need the tracker at 0 (because of out of range exception)
            _currentTotalFields = 0;

            if(_requireReadLine)
                this->_currentLine = _reader->read_line();
            while(this->HasNext())
            {
                StrT next = this->NextField();
                if(!(next == ""))
                    iteratorFunc(next);
            }
            _requireReadLine = true;

            return *this;
        }


        // Begins with 1
        /*!
         * \brief Read out (peeking) a field without going to the next line.
         */
        template<typename T>
        T ReadField(int fieldNum)
        {
            if(_requireReadLine)
                this->_currentLine = _reader->read_line();
            _requireReadLine = false;
            this->_currentCharIndex = 0;

            StrT field;
            for(int i = 1; i < fieldNum; i++)
            {
                if(this->_currentCharIndex >= StrTUtils::length(this->_currentLine))
                    throw std::logic_error("Expected " + std::to_string(fieldNum) + " CSV-Fields, got " + std::to_string(i - 1) + " @ line " + std::to_string(this->_lineTracker) + "!");

                this->SkipField();
            }
            field = this->NextField();

            T value;
            Converter::Convert(&value, field);
            return value;
        }

    };

    /*!
     * \brief Creates a new CSVReader.
     * \see CSVReader
     *
     * StrT template argument is the string class type.
     * StrTUtils is a wrapper for the StrT class providing following static functions:
     *      static bool find(const StrT& str, CharT sep, size_t& findIndex)
     *      static size_t length(const target_string& str)
     *      static target_string substring(const target_string& str, size_t pos, size_t count)
     *
     * Converter is a wrapper for converting StrT fields to literal types:
     *      template<typename T>
     *      static void Convert(T* out, const StrType& field)
     */
    template<class StrT, class StrTUtils, class Converter, class Reader, class CharT>
    constexpr CSVReader<Reader, StrT, CharT, StrTUtils, Converter> MakeCSVReader(Reader *reader, CharT /*sep*/)
    {
        return CSVReader<Reader, StrT, CharT, StrTUtils, Converter>(reader);
    }


    namespace detail
    {
        template<class Reader>
        struct CSVReaderFromReaderType
        {
            typedef typename Reader::string_type string_type;
            typedef typename string_type::value_type value_type;
            typedef typename string_type::traits_type traits_type;
            typedef typename string_type::allocator_type allocator_type;

            typedef CSVReader<Reader, string_type, value_type,
                    DefaultStringWrapper<value_type, traits_type, allocator_type>, DefaultCSVConverter<string_type>> full_type;
        };
    }

    /*!
     * \brief Creates a new CSVReader for STL strings. (std::string, std::wstring, ...)
     * \see CSVReader
     */
    template<class Reader, class CharT>
    constexpr typename detail::CSVReaderFromReaderType<Reader>::full_type MakeCSVReaderFromBasicString(Reader *reader, CharT sep)
    {
        typedef detail::CSVReaderFromReaderType<Reader> csv_reader_type;
        typedef typename csv_reader_type::value_type value_type;

        static_assert(std::is_same<CharT, value_type>::value, "Value type of basic_string must be the same as the type of the seperator!");
        return typename csv_reader_type::full_type(reader, sep);
    }


    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class... Values>
    struct CSVSubReader
    {
    public:
        CSVSubReader(CharT sep, bool isOptional, Values &&... allValues) : _sep(sep), _val(allValues...), _isOptional(isOptional)
        {}

        void ReadDataLine(const StrT &val)
        {
            ReadDataLineImpl(val, detail::make_index_sequence<sizeof...(Values)> {});
        }

        bool IsOptional() const
        {
            return _isOptional;
        }

    private:
        template<std::size_t ...I>
        void ReadDataLineImpl(const StrT &val, detail::index_sequence<I...>)
        {
            DirectReader<StrT> subReader(val);
            CSVReader<decltype(subReader), StrT, CharT, StrTUtils, Converter> subCSVReader(&subReader, _sep);
            subCSVReader.ReadDataLine(std::get<I>(_val)...);
        }

        CharT _sep;
        std::tuple<Values...> _val;
        bool _isOptional;
    };

    /*!
     * \brief Creates a new CSVSubReader.
     * \see CSVSubReader
     */
    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class SubChar, class... RestValues>
    constexpr CSVSubReader<Reader, StrT, CharT, StrTUtils, Converter, RestValues...> MakeCSVSubReader(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, SubChar sep, RestValues &&... values)
    {
        return CSVSubReader<Reader, StrT, CharT, StrTUtils, Converter, RestValues...>(sep, false, std::forward<RestValues>(values)...);
    }

    /*!
     * \brief Creates a new CSVSubReader, which is optional. It acts the same as CSVOptional
     * \see CSVSubReader
     * \see CSVOptional
     */
    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class SubChar, class... RestValues>
    constexpr CSVSubReader<Reader, StrT, CharT, StrTUtils, Converter, RestValues...> MakeCSVOptionalSubReader(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, SubChar sep, RestValues &&... values)
    {
        return CSVSubReader<Reader, StrT, CharT, StrTUtils, Converter, RestValues...>(sep, true, std::forward<RestValues>(values)...);
    }



    namespace detail
    {
        template<class ContainerT,                                                          // The container type
                 class StrT,                                                                // The string class
                 class CharT,                                                               // The char type
                 class StrTUtils,                                                           // The string util class
                 class Converter,
                 class PostProcessorFunc>
        struct CSVBatchReaderFromContainer
        {
            typedef typename ContainerT::value_type ContainerValueT;
            typedef CommonContainerUtils<ContainerValueT, ContainerT> ContainerUtils;

            typedef CSVBatchReader<ContainerValueT, ContainerT, ContainerUtils, StrT, CharT, StrTUtils, Converter, PostProcessorFunc> full_type;
        };
    }

    /*!
     * \brief Creates a new CSVBatchReader
     * \see CSVBatchReader
     *
     * An overload without PostProcessor
     */
    template<class ContainerT,                                                          // The container type
             class Reader,                                                              // The reader (not used)
             class StrT,                                                                // The string class
             class CharT,                                                               // The char type
             class StrTUtils,                                                           // The string util class
             class Converter>                                                           // The value converter
    constexpr typename detail::CSVBatchReaderFromContainer<ContainerT, StrT, CharT, StrTUtils, Converter, std::nullptr_t>::full_type
    MakeCSVBatchReader(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, CharT sep, ContainerT *container)
    {
        typedef detail::CSVBatchReaderFromContainer<ContainerT, StrT, CharT, StrTUtils, Converter, std::nullptr_t> csv_batch_reader_type;

        return typename csv_batch_reader_type::full_type(sep, container, nullptr);
    }

    /*!
     * \brief Creates a new CSVBatchReader
     * \see CSVBatchReader
     *
     * This will create a new CSVBatchReader. The ContainerT type is required to
     * expose the value type with ContainerT::value_type (STL compatible).
     *
     * In addition you can pass an optional PostProcessor function.
     */
    template<class ContainerT,                                                          // The container type
             class Reader,                                                              // The reader (not used)
             class StrT,                                                                // The string class
             class CharT,                                                               // The char type
             class SubCharT,
             class StrTUtils,                                                           // The string util class
             class Converter,                                                           // The value converter
             class PostProcessorFunc>
    constexpr typename detail::CSVBatchReaderFromContainer<ContainerT, StrT, CharT, StrTUtils, Converter, PostProcessorFunc>::full_type
    MakeCSVBatchReader(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, SubCharT sep, ContainerT *container, const PostProcessorFunc &postProcessorFunc)
    {
        typedef detail::CSVBatchReaderFromContainer<ContainerT, StrT, CharT, StrTUtils, Converter, PostProcessorFunc> csv_batch_reader_type;

        return typename csv_batch_reader_type::full_type(sep, container, postProcessorFunc);
    }


    /*!
     * \brief Creates a new CSVIterator
     * \see CSVIterator
     */
    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class SubChar, class IteratorFunc>
    constexpr CSVIterator<StrT, CharT, StrTUtils, Converter, IteratorFunc>
    MakeCSVIterator(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, SubChar sep, const IteratorFunc &iteratorFunc)
    {
        return CSVIterator<StrT, CharT, StrTUtils, Converter, IteratorFunc>(sep, false, iteratorFunc);
    }

    template<class Reader, class StrT, class CharT, class StrTUtils, class Converter, class SubChar, class IteratorFunc>
    constexpr CSVIterator<StrT, CharT, StrTUtils, Converter, IteratorFunc>
    MakeCSVOptionalIterator(const CSVReader<Reader, StrT, CharT, StrTUtils, Converter> &, SubChar sep, const IteratorFunc &iteratorFunc)
    {
        return CSVIterator<StrT, CharT, StrTUtils, Converter, IteratorFunc>(sep, true, iteratorFunc);
    }
}




#endif
