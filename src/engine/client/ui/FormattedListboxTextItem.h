#ifndef FORMATTEDLISTBOXTEXTITEM_H_
#define FORMATTEDLISTBOXTEXTITEM_H_

#include <CEGUI/CEGUI.h>

namespace CEGUI
{
//! A ListboxItem based class that can do horizontal text formatiing.
class FormattedListboxTextItem : public ListboxTextItem
{
public:
	//! Constructor
	FormattedListboxTextItem(const String& text,
			const HorizontalTextFormatting format = HTF_LEFT_ALIGNED,
			const uint item_id = 0,
			void* const item_data = 0,
			const bool disabled = false,
			const bool auto_delete = true);

	//! Destructor.
	~FormattedListboxTextItem();

	//! Return the current formatting set.
	HorizontalTextFormatting getFormatting() const;
	/*!
            Set the formatting.  You should call Listbox::handleUpdatedItemData
            after setting the formatting in order to update the listbox.  We do not
            do it automatically since you may wish to batch changes to multiple
            items and multiple calls to handleUpdatedItemData is wasteful.
	 */
	void setFormatting(const HorizontalTextFormatting fmt);

	// overriden functions.
	Sizef getPixelSize(void) const;
	void draw(GeometryBuffer& buffer, const Rectf& targetRect, float alpha, const Rectf* clipper) const;

protected:
	//! Helper to create a FormattedRenderedString of an appropriate type.
	void setupStringFormatter() const;
	//! Current formatting set
	HorizontalTextFormatting d_formatting;
	//! Class that renders RenderedString with some formatting.
	mutable FormattedRenderedString* d_formattedRenderedString;
	//! Tracks target area for rendering so we can reformat when needed
	mutable Sizef d_formattingAreaSize;
};

}

#endif /* FORMATTEDLISTBOXTEXTITEM_H_ */
