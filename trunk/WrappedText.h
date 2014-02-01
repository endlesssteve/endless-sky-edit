/* WrappedText.h
Michael Zahniser, 16 Oct 2013

Class for calculating word positions in wrapped text.
*/

#ifndef WRAPPED_TEXT_H_INCLUDED
#define WRAPPED_TEXT_H_INCLUDED

#include <string>
#include <vector>

class Color;
class Font;
class Point;



class WrappedText {
public:
	// The returned text is a series of words and (x, y) positions:
	class Word {
	public:
		const char *String() const;
		int X() const;
		int Y() const;
		
	private:
		const char *str;
		int x;
		int y;
		
		friend class WrappedText;
	};
	
	
public:
	WrappedText();
	WrappedText(const Font &font);
	~WrappedText();
	
	// Set the alignment mode.
	enum Align {LEFT, CENTER, RIGHT, JUSTIFIED};
	Align Alignment() const;
	void SetAlignment(Align align);
	
	// Set the wrap width. This does not include any margins.
	int WrapWidth() const;
	void SetWrapWidth(int width);
	
	// Set the font to use. This will also set sensible defaults for the tab
	// width, line height, and paragraph break. You must specify the wrap width
	// and the alignment separately.
	void SetFont(const Font &font);
	
	// Set the width in pixels of a single '\t' character.
	int TabWidth() const;
	void SetTabWidth(int width);
	
	// Set the height in pixels of one line of text within a paragraph.
	int LineHeight() const;
	void SetLineHeight(int height);
	
	// Set the extra spacing in pixels to be added in between paragraphs.
	int ParagraphBreak() const;
	void SetParagraphBreak(int height);
	
	// Get the word positions when wrapping the given text. The coordinates
	// always begin at (0, 0).
	const std::vector<Word> &Wrap(const std::string &str);
	const std::vector<Word> &Wrap(const char *str);
	
	// Get the words from the most recent wrapping.
	const std::vector<Word> &Words() const;
	// Get the height of the wrapped text.
	int Height() const;
	
	// Draw the text.
	void Draw(const Point &topLeft, const Color &color) const;
	
	
private:
	void SetText(const char *it, size_t length);
	void Wrap();
	void AdjustLine(unsigned &lineBegin, int &lineWidth, bool isEnd);
	int Space(char c) const;
	
	
private:
	const Font *font;
	
	int space;
	int wrapWidth;
	int tabWidth;
	int lineHeight;
	int paragraphBreak;
	Align alignment;
	
	char *text;
	std::vector<Word> words;
	int height;
};



#endif
