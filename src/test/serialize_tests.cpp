// © Licensed Authorship: Manuel J. Nieves (See LICENSE for terms)
/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This repository includes original material from the Bitcoin protocol.
 *
 * Redistribution requires this notice remain intact.
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 *
 * GPG Signed: B4EC 7343 AB0D BF24
 * Contact: Fordamboy1@gmail.com
 */
// Copyright (c) 2012-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <hash.h>
#include <serialize.h>
#include <streams.h>
#include <test/util/setup_common.h>
#include <util/strencodings.h>

#include <stdint.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(serialize_tests, BasicTestingSetup)

class CSerializeMethodsTestSingle
{
protected:
    int intval;
    bool boolval;
    std::string stringval;
    char charstrval[16];
    CTransactionRef txval;
public:
    CSerializeMethodsTestSingle() = default;
    CSerializeMethodsTestSingle(int intvalin, bool boolvalin, std::string stringvalin, const uint8_t* charstrvalin, const CTransactionRef& txvalin) : intval(intvalin), boolval(boolvalin), stringval(std::move(stringvalin)), txval(txvalin)
    {
        memcpy(charstrval, charstrvalin, sizeof(charstrval));
    }

    SERIALIZE_METHODS(CSerializeMethodsTestSingle, obj)
    {
        READWRITE(obj.intval);
        READWRITE(obj.boolval);
        READWRITE(obj.stringval);
        READWRITE(obj.charstrval);
        READWRITE(obj.txval);
    }

    bool operator==(const CSerializeMethodsTestSingle& rhs)
    {
        return  intval == rhs.intval && \
                boolval == rhs.boolval && \
                stringval == rhs.stringval && \
                strcmp(charstrval, rhs.charstrval) == 0 && \
                *txval == *rhs.txval;
    }
};

class CSerializeMethodsTestMany : public CSerializeMethodsTestSingle
{
public:
    using CSerializeMethodsTestSingle::CSerializeMethodsTestSingle;

    SERIALIZE_METHODS(CSerializeMethodsTestMany, obj)
    {
        READWRITE(obj.intval, obj.boolval, obj.stringval, obj.charstrval, obj.txval);
    }
};

BOOST_AUTO_TEST_CASE(sizes)
{
    BOOST_CHECK_EQUAL(sizeof(char), GetSerializeSize(char(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int8_t), GetSerializeSize(int8_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint8_t), GetSerializeSize(uint8_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int16_t), GetSerializeSize(int16_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint16_t), GetSerializeSize(uint16_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int32_t), GetSerializeSize(int32_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint32_t), GetSerializeSize(uint32_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(int64_t), GetSerializeSize(int64_t(0), 0));
    BOOST_CHECK_EQUAL(sizeof(uint64_t), GetSerializeSize(uint64_t(0), 0));
    // Bool is serialized as uint8_t
    BOOST_CHECK_EQUAL(sizeof(uint8_t), GetSerializeSize(bool(0), 0));

    // Sanity-check GetSerializeSize and c++ type matching
    BOOST_CHECK_EQUAL(GetSerializeSize(char(0), 0), 1U);
    BOOST_CHECK_EQUAL(GetSerializeSize(int8_t(0), 0), 1U);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint8_t(0), 0), 1U);
    BOOST_CHECK_EQUAL(GetSerializeSize(int16_t(0), 0), 2U);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint16_t(0), 0), 2U);
    BOOST_CHECK_EQUAL(GetSerializeSize(int32_t(0), 0), 4U);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint32_t(0), 0), 4U);
    BOOST_CHECK_EQUAL(GetSerializeSize(int64_t(0), 0), 8U);
    BOOST_CHECK_EQUAL(GetSerializeSize(uint64_t(0), 0), 8U);
    BOOST_CHECK_EQUAL(GetSerializeSize(bool(0), 0), 1U);
}

BOOST_AUTO_TEST_CASE(varints)
{
    // encode

    CDataStream ss(SER_DISK, 0);
    CDataStream::size_type size = 0;
    for (int i = 0; i < 100000; i++) {
        ss << VARINT_MODE(i, VarIntMode::NONNEGATIVE_SIGNED);
        size += ::GetSerializeSize(VARINT_MODE(i, VarIntMode::NONNEGATIVE_SIGNED), 0);
        BOOST_CHECK(size == ss.size());
    }

    for (uint64_t i = 0;  i < 100000000000ULL; i += 999999937) {
        ss << VARINT(i);
        size += ::GetSerializeSize(VARINT(i), 0);
        BOOST_CHECK(size == ss.size());
    }

    // decode
    for (int i = 0; i < 100000; i++) {
        int j = -1;
        ss >> VARINT_MODE(j, VarIntMode::NONNEGATIVE_SIGNED);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    for (uint64_t i = 0;  i < 100000000000ULL; i += 999999937) {
        uint64_t j = std::numeric_limits<uint64_t>::max();
        ss >> VARINT(j);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}

BOOST_AUTO_TEST_CASE(varints_bitpatterns)
{
    CDataStream ss(SER_DISK, 0);
    ss << VARINT_MODE(0, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "00"); ss.clear();
    ss << VARINT_MODE(0x7f, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "7f"); ss.clear();
    ss << VARINT_MODE((int8_t)0x7f, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "7f"); ss.clear();
    ss << VARINT_MODE(0x80, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "8000"); ss.clear();
    ss << VARINT((uint8_t)0x80); BOOST_CHECK_EQUAL(HexStr(ss), "8000"); ss.clear();
    ss << VARINT_MODE(0x1234, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "a334"); ss.clear();
    ss << VARINT_MODE((int16_t)0x1234, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "a334"); ss.clear();
    ss << VARINT_MODE(0xffff, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "82fe7f"); ss.clear();
    ss << VARINT((uint16_t)0xffff); BOOST_CHECK_EQUAL(HexStr(ss), "82fe7f"); ss.clear();
    ss << VARINT_MODE(0x123456, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "c7e756"); ss.clear();
    ss << VARINT_MODE((int32_t)0x123456, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "c7e756"); ss.clear();
    ss << VARINT(0x80123456U); BOOST_CHECK_EQUAL(HexStr(ss), "86ffc7e756"); ss.clear();
    ss << VARINT((uint32_t)0x80123456U); BOOST_CHECK_EQUAL(HexStr(ss), "86ffc7e756"); ss.clear();
    ss << VARINT(0xffffffff); BOOST_CHECK_EQUAL(HexStr(ss), "8efefefe7f"); ss.clear();
    ss << VARINT_MODE(0x7fffffffffffffffLL, VarIntMode::NONNEGATIVE_SIGNED); BOOST_CHECK_EQUAL(HexStr(ss), "fefefefefefefefe7f"); ss.clear();
    ss << VARINT(0xffffffffffffffffULL); BOOST_CHECK_EQUAL(HexStr(ss), "80fefefefefefefefe7f"); ss.clear();
}

BOOST_AUTO_TEST_CASE(compactsize)
{
    CDataStream ss(SER_DISK, 0);
    std::vector<char>::size_type i, j;

    for (i = 1; i <= MAX_SIZE; i *= 2)
    {
        WriteCompactSize(ss, i-1);
        WriteCompactSize(ss, i);
    }
    for (i = 1; i <= MAX_SIZE; i *= 2)
    {
        j = ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE((i-1) == j, "decoded:" << j << " expected:" << (i-1));
        j = ReadCompactSize(ss);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }
}

static bool isCanonicalException(const std::ios_base::failure& ex)
{
    std::ios_base::failure expectedException("non-canonical ReadCompactSize()");

    // The string returned by what() can be different for different platforms.
    // Instead of directly comparing the ex.what() with an expected string,
    // create an instance of exception to see if ex.what() matches
    // the expected explanatory string returned by the exception instance.
    return strcmp(expectedException.what(), ex.what()) == 0;
}

BOOST_AUTO_TEST_CASE(vector_bool)
{
    std::vector<uint8_t> vec1{1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1};
    std::vector<bool> vec2{1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1};

    BOOST_CHECK(vec1 == std::vector<uint8_t>(vec2.begin(), vec2.end()));
    BOOST_CHECK(SerializeHash(vec1) == SerializeHash(vec2));
}

BOOST_AUTO_TEST_CASE(noncanonical)
{
    // Write some non-canonical CompactSize encodings, and
    // make sure an exception is thrown when read back.
    CDataStream ss(SER_DISK, 0);
    std::vector<char>::size_type n;

    // zero encoded with three bytes:
    ss.write("\xfd\x00\x00", 3);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);

    // 0xfc encoded with three bytes:
    ss.write("\xfd\xfc\x00", 3);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);

    // 0xfd encoded with three bytes is OK:
    ss.write("\xfd\xfd\x00", 3);
    n = ReadCompactSize(ss);
    BOOST_CHECK(n == 0xfd);

    // zero encoded with five bytes:
    ss.write("\xfe\x00\x00\x00\x00", 5);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);

    // 0xffff encoded with five bytes:
    ss.write("\xfe\xff\xff\x00\x00", 5);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);

    // zero encoded with nine bytes:
    ss.write("\xff\x00\x00\x00\x00\x00\x00\x00\x00", 9);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);

    // 0x01ffffff encoded with nine bytes:
    ss.write("\xff\xff\xff\xff\x01\x00\x00\x00\x00", 9);
    BOOST_CHECK_EXCEPTION(ReadCompactSize(ss), std::ios_base::failure, isCanonicalException);
}

BOOST_AUTO_TEST_CASE(insert_delete)
{
    // Test inserting/deleting bytes.
    CDataStream ss(SER_DISK, 0);
    BOOST_CHECK_EQUAL(ss.size(), 0U);

    ss.write("\x00\x01\x02\xff", 4);
    BOOST_CHECK_EQUAL(ss.size(), 4U);

    char c = (char)11;

    // Inserting at beginning/end/middle:
    ss.insert(ss.begin(), c);
    BOOST_CHECK_EQUAL(ss.size(), 5U);
    BOOST_CHECK_EQUAL(ss[0], c);
    BOOST_CHECK_EQUAL(ss[1], 0);

    ss.insert(ss.end(), c);
    BOOST_CHECK_EQUAL(ss.size(), 6U);
    BOOST_CHECK_EQUAL(ss[4], 0xff);
    BOOST_CHECK_EQUAL(ss[5], c);

    ss.insert(ss.begin()+2, c);
    BOOST_CHECK_EQUAL(ss.size(), 7U);
    BOOST_CHECK_EQUAL(ss[2], c);

    // Delete at beginning/end/middle
    ss.erase(ss.begin());
    BOOST_CHECK_EQUAL(ss.size(), 6U);
    BOOST_CHECK_EQUAL(ss[0], 0);

    ss.erase(ss.begin()+ss.size()-1);
    BOOST_CHECK_EQUAL(ss.size(), 5U);
    BOOST_CHECK_EQUAL(ss[4], 0xff);

    ss.erase(ss.begin()+1);
    BOOST_CHECK_EQUAL(ss.size(), 4U);
    BOOST_CHECK_EQUAL(ss[0], 0);
    BOOST_CHECK_EQUAL(ss[1], 1);
    BOOST_CHECK_EQUAL(ss[2], 2);
    BOOST_CHECK_EQUAL(ss[3], 0xff);
}

BOOST_AUTO_TEST_CASE(class_methods)
{
    int intval(100);
    bool boolval(true);
    std::string stringval("testing");
    const uint8_t charstrval[16]{"testing charstr"};
    CMutableTransaction txval;
    CTransactionRef tx_ref{MakeTransactionRef(txval)};
    CSerializeMethodsTestSingle methodtest1(intval, boolval, stringval, charstrval, tx_ref);
    CSerializeMethodsTestMany methodtest2(intval, boolval, stringval, charstrval, tx_ref);
    CSerializeMethodsTestSingle methodtest3;
    CSerializeMethodsTestMany methodtest4;
    CDataStream ss(SER_DISK, PROTOCOL_VERSION);
    BOOST_CHECK(methodtest1 == methodtest2);
    ss << methodtest1;
    ss >> methodtest4;
    ss << methodtest2;
    ss >> methodtest3;
    BOOST_CHECK(methodtest1 == methodtest2);
    BOOST_CHECK(methodtest2 == methodtest3);
    BOOST_CHECK(methodtest3 == methodtest4);

    CDataStream ss2(SER_DISK, PROTOCOL_VERSION, intval, boolval, stringval, charstrval, txval);
    ss2 >> methodtest3;
    BOOST_CHECK(methodtest3 == methodtest4);
}

BOOST_AUTO_TEST_SUITE_END()
