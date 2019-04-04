#pragma once

namespace hackPSU {

    class MenuItem;

    class MenuAction{
    public:
        MenuAction();
        virtual void select() = 0;
        virtual String display() = 0;
    };

    class MenuItem : public MenuAction{
    public:
        String display() { return name; }
        void select() { onSelect(); }
        MenuItem* next() { return next;}
        MenuItem* previous() { return previous;}
    private:
        String name;
        MenuItem* previous;
        MenuItem* next;
        void (*onSelect)();

    };

    class MenuGroup : public MenuItem {
    public:
        MenuGroup();

        bool addMenuItem(MenuItem*);
        void select();
        String display() { return enter ? current->display() : name + ".."; }
    private:
        String name;
        MenuItem* head;
        MenuItem* current;
        bool enter;

    };


}