#ifndef DRIVECOMMAND_H
#define DRIVECOMMAND_H
#include <vector>
#include <memory>

#include "wheelsendmessage.h"
#include "command.h"


/**
 * @brief The DriveCommand class is a root class to create a set of chassis commands.
 */
class DriveCommand : public Command<std::vector<WheelSendMessage>>
{
public:
    /**
     * @brief Destructor
     */
    ~DriveCommand() override;
};



template<typename ConvertFrom, typename ConvertTo, typename ConvertFunction>
class DriveCommandConverter : public Command<ConvertTo>
{
    ConvertFrom data;
    ConvertFunction convert;
public:
    template<typename CvtFromData, typename CvtFunction>
    DriveCommandConverter(CvtFromData &&data, CvtFunction &&cf)
        : data{std::forward<CvtFromData>(data)}, convert{cf}
    {}

    ConvertTo execute()
    {
        return convert(data);
    }

};

#endif // DRIVECOMMAND_H
