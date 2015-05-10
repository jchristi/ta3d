
#include "unitselector.h"
#include "../../misc/paths.h"
#include "../../misc/files.h"
#include <algorithm>
#include "../../fbi.h"
#include "../../logs/logs.h"
#include "../../languages/i18n.h"




namespace TA3D
{
namespace Menus
{

    bool UnitSelector::Execute(const String& preSelectedUnits, String& useOnly)
    {
        UnitSelector m(preSelectedUnits);
        bool r = m.execute();
        useOnly = (r) ? useOnly = m.selected() : preSelectedUnits;
        return (r && (!useOnly.empty()));
    }





    UnitSelector::UnitSelector()
        :Abstract(),
        pUseOnly(),
        pDefaultUseOnly(),
        pUnitPicObj(NULL),
        pUnitListObj(NULL),
        pLastUnitIndex(-1)
    {}


    UnitSelector::UnitSelector(const String& preSelectedUnits)
        :Abstract(),
        pUseOnly(preSelectedUnits),
        pDefaultUseOnly(preSelectedUnits),
        pUnitPicObj(NULL),
        pUnitListObj(NULL),
        pLastUnitIndex(-1)
    {}


    UnitSelector::~UnitSelector()
    {
        unit_manager.destroy();
    }
    

    bool UnitSelector::doInitialize()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_UNITSELECTOR << "Entering...");

        loadAreaFromTDF("unit setup", "gui/unitsetup.area");
        // Load all units, if any
        load_all_units();
        if (!unit_manager.nb_unit) // should abort if no map is present
            return false;

        if (!pUseOnly.empty())          // Load previous selection
        {
            cTAFileParser useonly_parser(pUseOnly, false, false, true); // In gadgets mode so we can read the special key :)
            for (int i = 0; i < unit_manager.nb_unit ; i++)
                unit_manager.unit_type[i]->not_used = true;
            String unit_name;
            int i = 0;
            while (!(unit_name = useonly_parser.pullAsString(String::Format("gadget%d", i))).empty())
            {
                int idx = unit_manager.get_unit_index( unit_name.c_str());
                if (idx >= 0)
                    unit_manager.unit_type[idx]->not_used = false;
                ++i;
            }
        }

        pUnitList.clear();
        for (int i = 0 ; i < unit_manager.nb_unit ; ++i)
            pUnitList.push_back( unit_manager.unit_type[i]->Unitname );
        pUnitList.sort();

        // The mini map object
        pUnitPicObj = pArea->get_object("unitsetup.unitpic");
        if (!pUnitPicObj)
            LOG_ERROR(LOG_PREFIX_MENU_UNITSELECTOR << "Can not find the GUI object `unitsetup.unitpic`");
        else
        {
            pUnitPicObj->Data = 0;
        }

        // The control which contains all available maps
        pUnitListObj = pArea->get_object("unitsetup.unit_list");
        reloadUnitsForGUIControl();
        doGoSelectSingleUnit(0);

        return true;
    }
        
    void UnitSelector::reloadUnitsForGUIControl()
    {
        if (pUnitListObj)
        {
            // Load all units
            pUnitListObj->Text.resize(pUnitList.size());
            String::List::iterator it = pUnitList.begin();
            for (int i = 0 ; i < pUnitList.size() ; ++i, ++it)
            {
                pUnitListObj->Text[i] = *it;
                int type_id = unit_manager.get_unit_index( *it );
                if (type_id >= 0 && !unit_manager.unit_type[type_id]->not_used)
                    pUnitListObj->Text[i] = "<H>" + pUnitListObj->Text[i];
            }
        }
    }


    void UnitSelector::doFinalize()
    {
        LOG_DEBUG(LOG_PREFIX_MENU_UNITSELECTOR << "Done.");
    }


    void UnitSelector::waitForEvent()
    {
        bool keyIsPressed(false);
        do
        {
            // Get if a key was pressed
            keyIsPressed = keypressed();
            // Grab user events
            pArea->check();
            // Wait to reduce CPU consumption 
            rest(TA3D_MENUS_RECOMMENDED_TIME_MS_FOR_RESTING);

        } while (pMouseX == mouse_x && pMouseY == mouse_y && pMouseZ == mouse_z && pMouseB == mouse_b
                 && mouse_b == 0
                 && !key[KEY_ENTER] && !key[KEY_ESC] && !key[KEY_SPACE] && !key[KEY_C]
                 && !keyIsPressed && !pArea->scrolling);
    }


    bool UnitSelector::maySwitchToAnotherMenu()
    {
        // Aborting
        if (pArea->get_state("unitsetup.b_cancel") || key[KEY_ESC])
        {
            pUseOnly = pDefaultUseOnly;
            if (pUnitListObj)
                pUnitListObj->Data = 0;
            return true;
        }

        // Go ! Validate selection !
        if (pArea->get_state("unitsetup.b_ok") || key[KEY_ENTER])
        {
            if (pUnitListObj)
            {
                pUnitListObj->Data = 0;
                createUseOnlyFile();
            }
            return true;
        }

        // Enable/Disable selected unit ! (require pUnitListObj != NULL)
        if (pUnitListObj && pLastUnitIndex >= 0)
        {
            if (pArea->get_state("unitsetup.c_enabled"))        // Enable
            {
                String UnitName = pUnitListObj->Text[ pLastUnitIndex ];
                if (UnitName.size() > 0 && UnitName[0] != '<')
                {
                    UnitName = "<H>" + UnitName;
                    pUnitListObj->Text[ pLastUnitIndex ] = UnitName;
                }
            }
            else                                                // Disable
            {
                String UnitName = pUnitListObj->Text[ pLastUnitIndex ];
                if (UnitName.size() > 0 && UnitName[0] == '<')
                {
                    UnitName = UnitName.substr( 3, UnitName.size() - 3);
                    pUnitListObj->Text[ pLastUnitIndex ] = UnitName;
                }
            }
        }

        // Selection change
        if (pUnitListObj)
            return doGoSelectSingleUnit(pUnitListObj->Pos);

        return false;
    }


    bool UnitSelector::doGoSelectSingleUnit(const int unitIndex)
    {
        // Bounds checking
        if (unitIndex < 0 || unitIndex >= unit_manager.nb_unit || pLastUnitIndex == unitIndex)
            return false;

        // Cached value
        pLastUnitIndex = unitIndex;

        // Logs
        LOG_DEBUG(LOG_PREFIX_MENU_UNITSELECTOR << "`" << pUnitListObj->Text[ unitIndex ]
                  << "` has been selected (indx: " << unitIndex << ")");

        // Update unit info
        doUpdateUnitInfo();

        return false;
    }


    void UnitSelector::doUpdateUnitInfo()
    {
        if (pUnitPicObj)
        {
            String UnitName = pUnitListObj->Text[ pLastUnitIndex ];
            if (UnitName.size() > 0 && UnitName[0] == '<')
            {
                UnitName = UnitName.substr( 3, UnitName.size() - 3);
                pArea->set_state("unitsetup.c_enabled", true);
            }
            else
                pArea->set_state("unitsetup.c_enabled", false);
            int type_id = unit_manager.get_unit_index( UnitName );
            if (type_id >= 0)
            {
                pUnitPicObj->Data = unit_manager.unit_type[type_id]->glpic;

                String info_string;
                info_string << unit_manager.unit_type[type_id]->name << "\n\n";
                info_string << unit_manager.unit_type[type_id]->Description << "\n";
                pArea->set_caption("unitsetup.unit_info", info_string);
            }
        }
    }

    void UnitSelector::createUseOnlyFile()
    {
        TA3D::Paths::MakeDir( TA3D::Paths::Resources + "useonly" );
        pUseOnly = "useonly/useonly.tdf";
        String filename = TA3D::Paths::Resources + pUseOnly;
        
        String s;
        s << "// Use Only file\n"
        << "// Auto-generated by ta3d: " << Logs::info.date() << "\n"
        << "\n";
        
        for(int i = 0 ; i < pUnitListObj->Text.size() ; ++i)            // For each selected unit
            if (pUnitListObj->Text[i][0] == '<')                        // create an empty section with the unit name
            {
                String UnitName = pUnitListObj->Text[i];
                UnitName = UnitName.substr( 3, UnitName.size() - 3 );
                s   << "[" << UnitName << "]\n"
                    << "{\n}\n";
            }

        if (Paths::Files::SaveToFile(filename, s))
        {
            LOG_INFO(LOG_PREFIX_MENU_UNITSELECTOR << "The useonly file has been saved.");
            return;
        }
        LOG_ERROR(LOG_PREFIX_MENU_UNITSELECTOR << "Impossible to write useonly file: `" << filename << "`");
    }

} // namespace Menus
} // namespace TA3D


