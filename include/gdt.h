#ifndef __NBSOS__GDT_H
#define __NBSOS__GDT_H

#include <common/types.h>

namespace NBSOS
{
    class GlobalDescriptorTable
    {
        public:
            class SegmentDescriptor
            {
                private:
                    Common::uint16_t limit_lo;
                    Common::uint16_t base_lo;
                    Common::uint8_t base_hi;
                    Common::uint8_t type;
                    Common::uint8_t flags_limit_hi;
                    Common::uint8_t base_vhi;
                public:
                    SegmentDescriptor(Common::uint32_t base, Common::uint32_t limit, Common::uint8_t type);
                    Common::uint32_t base();
                    Common::uint32_t limit();
            }; __attribute__((packed)); // we need to be byte perfect here
        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;
        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            Common::uint16_t CodeSegmentSelector();
            Common::uint16_t DataSegmentSelector();
    };
}
#endif