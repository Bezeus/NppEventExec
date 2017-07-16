/*
This file is part of NppEventExec
Copyright (C) 2016-2017 Mihail Ivanchev

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "test.h"
#include "event_map.h"
#include "csv.h"
#include "mem.h"
#include "util.h"
#include "Notepad_plus_msgs.h"

declare_assert(file_open, int fieldCnt, int header);
declare_assert(file_read);
declare_assert(any_str_read);
declare_assert(bool_read, bool val);
declare_assert(event_read, unsigned int val);
declare_assert(str_read, const wchar_t *val);
declare_assert(char_and_unit_length, size_t charCnt, size_t unitCnt);

#define assert_file_open(fieldCnt, header) \
    call_assert_proc(file_open, fieldCnt, header)
#define assert_file_read()     call_assert_proc(file_read)
#define assert_any_str_read()  call_assert_proc(any_str_read)
#define assert_bool_read(val)  call_assert_proc(bool_read, val)
#define assert_event_read(val) call_assert_proc(event_read, val)
#define assert_str_read(val)   call_assert_proc(str_read, val)
#define assert_char_and_unit_length(charCnt, unitCnt) \
    call_assert_proc(char_and_unit_length, charCnt, unitCnt);

#define assert_success()                                   \
    if (csvOpen(L"csv\\success.csv", 2, 1))                \
        cr_fail("Failed to open the success test file.");  \
                                                           \
    assert_any_str_read();                                 \
    assert_any_str_read();                                 \
    assert_any_str_read();                                 \
    assert_any_str_read();                                 \
    assert_any_str_read();                                 \
    assert_any_str_read();                                 \
    assert_file_read()

#define assert_failure()                                                    \
    if (csvOpen(L"csv\\failure.csv", 2, 1))                                 \
        cr_fail("Failed to open the failure test file.");                   \
                                                                            \
    assert_any_str_read();                                                  \
    assert_any_str_read();                                                  \
                                                                            \
    if ((val = csvReadString(&charCnt, &unitCnt)))                          \
    {                                                                       \
        csvClose();                                                         \
        cr_fail("Expected syntax error because of an invalid column "       \
                "separator did not occur; a string was correctly parsed."); \
    }                                                                       \
                                                                            \
    csvClose()

static void fini(void);

TestSuite(csv, .fini = fini);

Test(csv, success_after_failure)
{
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_failure();
    assert_success();
}

Test(csv, failure_after_success) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_success();
    assert_failure();
}

Test(csv, success_after_success) {
    assert_success();
    assert_success();
}

Test(csv, nl_at_eof) {
    assert_file_open(2, 1);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_file_read();
}

Test(csv, no_nl_at_eof) {
    assert_file_open(2, 1);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_file_read();
}

Test(csv, cr_instead_of_crlf) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(2, 1);
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of an malformed newline "
                "sequence did not occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, lf_instead_of_crlf) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(2, 1);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of an malformed newline "
                "sequence did not occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, less_columns_than_expected) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(4, 0);
    assert_any_str_read();
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of a wrong number of columns "
                "did not occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, more_columns_than_expected) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(4, 0);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of a wrong number of columns "
                "did not occur; a string was correctly parsed.");
    }

    csvClose();

}

Test(csv, space_before_quote) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(3, 1);
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of whitespace before an "
                "opening quote did not occur; a string was correctly "
                "parsed.");
    }

    csvClose();
}

Test(csv, space_after_quote) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(3, 1);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();

    if ((val = csvReadString(&charCnt, &unitCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of whitespace after a closing "
                "quote did not occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, bools)
{
    assert_file_open(1, 1);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(true);
    assert_bool_read(false);
    assert_bool_read(false);
    assert_file_read();
}

Test(csv, events)
{
    assert_file_open(1, 1);
    assert_event_read(NPPN_READY);
    assert_event_read(NPPN_FILEBEFORECLOSE);
    assert_event_read(NPPN_FILECLOSED);
    assert_event_read(NPPN_FILEBEFOREOPEN);
    assert_event_read(NPPN_FILEOPENED);
    assert_event_read(NPPN_FILEBEFORESAVE);
    assert_event_read(NPPN_FILESAVED);
    assert_event_read(NPPN_FILEBEFORELOAD);
    assert_event_read(NPPN_FILELOADFAILED);
    assert_event_read(NPPN_FILEBEFOREDELETE);
    assert_event_read(NPPN_FILEDELETEFAILED);
    assert_event_read(NPPN_FILEDELETED);
    assert_event_read(NPPN_FILEBEFORERENAME);
    assert_event_read(NPPN_FILERENAMECANCEL);
    assert_event_read(NPPN_FILERENAMED);
    assert_event_read(NPPN_BEFORESHUTDOWN);
    assert_event_read(NPPN_CANCELSHUTDOWN);
    assert_event_read(NPPN_SHUTDOWN);
    assert_event_read(NPPN_BUFFERACTIVATED);
    assert_event_read(NPPN_LANGCHANGED);
    assert_event_read(NPPN_WORDSTYLESUPDATED);
    assert_event_read(NPPN_SHORTCUTREMAPPED);
    assert_event_read(NPPN_READONLYCHANGED);
    assert_event_read(NPPN_DOCORDERCHANGED);
    assert_event_read(NPPN_SNAPSHOTDIRTYFILELOADED);
    assert_file_read();
}

Test(csv, strings)
{
    assert_file_open(1, 1);
    assert_str_read(L"The quick brown fox jumped over\r\nthe lazy\r\ndog.");
    assert_str_read(L"Z");
    assert_str_read(L"Überhitzung der Verbindungselemente könnte zu Bränden in "
                    "den Versorgungsschächten führen.");
    assert_str_read(L"Ich konnte mich eigentlich nie mit der \"Neuen Deutschen "
                    L"Welle\" identifizieren, denn...\r\nvieles aus der Zeit "
                    L"klang einfach nicht gut.");
    assert_str_read(L"Бетонни прегради по строежите създават допълнителни "
                    L"рискове от наранявания!");
    assert_str_read(L"Обиди като\r\n\"тъпак\" и\r\n\"глупак\"\r\nса често "
                    L"срещани в модерния език.");
    assert_str_read(L"祝你生日快乐！");
    assert_str_read(L"ἀνδρῶν γὰρ ἐπιφανῶν πᾶσα γῆ τάφος");
    assert_str_read(L"Не то мудрено, что переговорено, а то, что не "
                    L"договорено.");
    assert_str_read(L"");
    assert_str_read(L" ");
    assert_str_read(L"					");
    assert_str_read(L"  \r\n		\r\n \r\n   ");
    assert_str_read(L"h");
    assert_str_read(L"L o L o L");
    assert_str_read(L"0123456789");
    assert_str_read(L"!*=-){__>>]4322+€µ");
    assert_str_read(L"真 SAMURAI SPIRITS 覇王丸地獄変");
    assert_file_read();
}

Test(csv, mixed) {
    assert_file_open(6, 1);
    assert_bool_read(true);
    assert_event_read(NPPN_SHUTDOWN);
    assert_str_read(L"SOMETHING WENT WRONG!\r\n\r\nABORT\r\nRETRY\r\nCANCEL\r\n");
    assert_bool_read(false);
    assert_event_read(NPPN_CANCELSHUTDOWN);
    assert_str_read(L"");
    assert_bool_read(true);
    assert_event_read(NPPN_DOCORDERCHANGED);
    assert_str_read(L"Failed to report a changed document order.");
    assert_bool_read(true);
    assert_event_read(NPPN_SHORTCUTREMAPPED);
    assert_str_read(L"A shortcut remapping will go unnoticed...");
    assert_bool_read(true);
    assert_event_read(NPPN_FILERENAMED);
    assert_str_read(L"Изпращането на съобщение за преименуване на файл беше "
                    L"неуспешно.");
    assert_bool_read(false);
    assert_event_read(NPPN_FILECLOSED);
    assert_str_read(L"");
    assert_file_read();
}

Test(csv, char_and_unit_length, .disabled=true) {
/*
    assert_file_open(3, 1);
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_char_and_unit_length();
    assert_file_read();
*/
}

Test(csv, unescaped_quote)
{
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(2, 1);
    assert_any_str_read();

    if ((val = csvReadString(&unitCnt, &charCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of an unescaped quote did not "
                "occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, unmatched_quote)
{
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(3, 0);
    assert_any_str_read();

    if ((val = csvReadString(&unitCnt, &charCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected syntax error because of an unmatched quote did not "
                "occur; a string was correctly parsed.");
    }

    csvClose();
}

Test(csv, invalid_utf8_seq)
{
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    assert_file_open(3, 0);
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();
    assert_any_str_read();

    if ((val = csvReadString(&unitCnt, &charCnt)))
    {
        freeStr(val);
        csvClose();
        cr_fail("Expected error because of an invalid UTF-8 encoding sequence "
                "did not occur; a string was correctly parsed.");
    }

    csvClose();
}

define_assert(file_open, int fieldCnt, int header) {
    char *name;
    wchar_t path[2048];
    size_t res;

    assert(BUFLEN(path) > 1);

    name = (char*) criterion_current_test->name;
    if (StringCbPrintfW(path, sizeof(path), L"csv\\%S.csv", name) != S_OK)
    {
        cr_assert_failure("The test name is unreasonably long or the path "
                          "buffer is unrealistically small.");
    }

    if (csvOpen(path, fieldCnt, header))
        cr_assert_failure("Failed to open the test file.");
}

define_assert(file_read) {
    if (csvHasData())
    {
        csvClose();
        cr_assert_failure("The CSV file contains unparsed data.");
    }

    csvClose();
}

define_assert(any_str_read) {
    wchar_t *val;
    size_t unitCnt;
    size_t charCnt;

    if (!(val = csvReadString(&unitCnt, &charCnt)))
    {
        csvClose();
        cr_assert_failure("Failed to read a string value.");
    }

    freeStr(val);
}

define_assert(bool_read, bool val) {
    int res;

    if ((res = csvReadBool()) == -1)
    {
        csvClose();
        cr_assert_failure("Failed to read a boolean value.");
    }
    else if (val != res)
    {
        csvClose();
        cr_assert_failure(
            "Read the boolean value \"%ls\", but \"%ls\" was expected.",
            BOOL_TO_STR_TRUE_FALSE(res),
            BOOL_TO_STR_TRUE_FALSE(val));
    }
}

define_assert(event_read, unsigned int val) {
    unsigned int event;

    if (csvReadEvent(&event))
    {
        csvClose();
        cr_assert_failure("Failed to read an event.");
    }
    else if (val != event)
    {
        csvClose();
        cr_assert_failure(
            "Read the Notepad++ event %ls, but %ls was expected.",
            getEventMapEntry(event)->name,
            getEventMapEntry(val)->name);
    }
}

define_assert(str_read, const wchar_t *val) {
    wchar_t *str;
    size_t charCnt;
    size_t unitCnt;

    if (!(str = csvReadString(&charCnt, &unitCnt)))
    {
        csvClose();
        cr_assert_failure("Failed to read a string.");
    }
    if (wcscmp(val, str))
    {
        freeStr(str);
        csvClose();
        cr_assert_failure("Read the string \"%ls\", but \"%ls\" was expected.",
                          str, val);
    }

    freeStr(str);
}

define_assert(char_and_unit_length, size_t charCnt, size_t unitCnt) {
    wchar_t *str;
    size_t charCntActual;
    size_t unitCntActual;

    if (!(str = csvReadString(&charCnt, &unitCnt)))
    {
        csvClose();
        cr_assert_failure("Failed to read a string.");
    }

    freeStr(str);

    if (charCnt != charCntActual)
    {
        csvClose();
        cr_assert_failure("Read a string consisting of %zu characters, but a "
                          "length of %zu characters was expected.",
                          charCntActual, charCnt);
    }
    if (unitCnt != unitCntActual)
    {
        csvClose();
        cr_assert_failure("Read a string consisting of %zu UTF-16 units, but a "
                          "length of %zu UTF-16 units was expected.",
                          unitCntActual, unitCnt);
    }

    csvClose();
}

void fini(void)
{
#ifdef DEBUG
    if (allocatedBytes)
    {
        cr_log_error("%lu bytes were not deallocated after the test.",
                     allocatedBytes);
        abort();
    }
#endif
}
