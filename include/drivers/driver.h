#ifndef __NBSOS__DRIVERS__DRIVER_H
#define __NBSOS__DRIVERS__DRIVER_H
namespace NBSOS
{
    namespace Drivers
    {
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
    }
}
#endif