#ifndef __DRIVER_H
#define __DRIVER_H

    class Driver
    {
        public:
            Driver();
            ~Driver();

            virtual void activate();
            virtual int reset();
            virtual void deactivate();
    };

    class DriverManager
    {
        private:
            Driver* drivers[256];
            int numDrivers;
        public:
            DriverManager();
            ~DriverManager();

            void addDriver(Driver* driver);
            void activateAll();
    };
#endif