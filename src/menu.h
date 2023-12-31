#ifndef MENU_H
#define MENU_H

class cMenuItemBase {
    public:
        enum eMenuState {IDLE, EDIT};
        virtual void        getText(char *text, size_t *cursor_start, size_t *cursor_stop);
        virtual void        getHelp(char *help);
        virtual eMenuState  eventHandler(uint8_t event);
        virtual void        select();
};

class cMenuItemInt : cMenuItemBase {
    public:
        cMenuItemInt(const char *label, const char *help, int *pIntVar, int min, int max); 

        void        getText(char *text, size_t *cursor_start, size_t *cursor_stop) override;
        void        getHelp(char *help) override;
        cMenuItemBase::eMenuState  eventHandler(uint8_t event) override;
        void        select() override;

    private:
        eMenuState _state;
        const char *_label, *_help;
        int *_value;
        size_t _ndigits;
        char _format[6];
        int _min, _max;
        size_t _cursor;

        void        reset();
};

class cMenuItemFunc : cMenuItemBase {
    public:
        cMenuItemFunc(const char *label, const char *help, void (*callback)(void *), void *param);
        void        getText(char *text, size_t *cursor_start, size_t *cursor_stop) override;
        void        getHelp(char *help) override;
        cMenuItemBase::eMenuState  eventHandler(uint8_t event) override;
        void        select() override;

    private:
        const char *_label, *_help;
        void (*_callback)(void *);
        void *_param;
        unsigned long _lastSaveTime;
};

#endif