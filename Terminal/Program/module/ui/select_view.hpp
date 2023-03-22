#ifndef MODULE_UI_SELECT_VIEW_HPP
#define MODULE_UI_SELECT_VIEW_HPP

#include "../core/button.hpp"
#include "../core/display.hpp"

namespace ui
{
class SelectView
{
  public:
    explicit SelectView(
        const module::Display& display,
        const size_t item_count,
        const String item_list[],
        const int8_t line_offset = 0)
        : DISPLAY_(display), ITEM_LIST_LENGTH_(item_count), ITEM_LIST_(item_list), LINE_OFFSET_(line_offset) {
    }

    void show() const {
        for (size_t i = 0; i < ITEM_LIST_LENGTH_; ++i) {
            DISPLAY_.drawStringLine(ITEM_LIST_[i], LINE_OFFSET_ + i, i == current_index_);
        }
    }

    void next() {
        DISPLAY_.drawStringLine(ITEM_LIST_[current_index_], LINE_OFFSET_ + current_index_, false);
        current_index_ = (current_index_ + 1) % ITEM_LIST_LENGTH_;
        DISPLAY_.drawStringLine(ITEM_LIST_[current_index_], LINE_OFFSET_ + current_index_, true);
    }

    void prev() {
        DISPLAY_.drawStringLine(ITEM_LIST_[current_index_], LINE_OFFSET_ + current_index_, false);
        current_index_ = (current_index_ - 1 + ITEM_LIST_LENGTH_) % ITEM_LIST_LENGTH_;
        DISPLAY_.drawStringLine(ITEM_LIST_[current_index_], LINE_OFFSET_ + current_index_, true);
    }

    size_t get() const {
        return current_index_;
    }

  private:
    const module::Display& DISPLAY_;
    const size_t ITEM_LIST_LENGTH_;
    const String* ITEM_LIST_;
    const int8_t LINE_OFFSET_;
    size_t current_index_ = 0;
};
}  // namespace ui

#endif
