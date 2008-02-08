#include <sys/systm.h>

#include "RemapUtil.hpp"
#include "keycode.hpp"
#include "config.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  AllFlagStatus allFlagStatus;
  ListFireExtraKey listFireExtraKey;
  ModifierCanceling modifierCanceling_control(ModifierFlag::CONTROL_L);
  ModifierCanceling modifierCanceling_option(ModifierFlag::OPTION_L);

  bool
  RemapUtil::isModifierOn(const RemapParams &params, ModifierFlag::ModifierFlag flag)
  {
    return ((*(params.flags) & flag) == flag);
  }

  KeyCode::KeyCode
  RemapUtil::getModifierKeyCode(ModifierFlag::ModifierFlag flag)
  {
    if (flag == ModifierFlag::CAPSLOCK) return KeyCode::CAPSLOCK;
    if (flag == ModifierFlag::SHIFT_L) return KeyCode::SHIFT_L;
    if (flag == ModifierFlag::SHIFT_R) return KeyCode::SHIFT_R;
    if (flag == ModifierFlag::CONTROL_L) return KeyCode::CONTROL_L;
    if (flag == ModifierFlag::CONTROL_R) return KeyCode::CONTROL_R;
    if (flag == ModifierFlag::OPTION_L) return KeyCode::OPTION_L;
    if (flag == ModifierFlag::OPTION_R) return KeyCode::OPTION_R;
    if (flag == ModifierFlag::COMMAND_L) return KeyCode::COMMAND_L;
    if (flag == ModifierFlag::COMMAND_R) return KeyCode::COMMAND_R;
    if (flag == ModifierFlag::FN) return KeyCode::FN;

    IOLog("KeyRemap4MacBook::getModifierKeyCode invalid args\n");
    return KeyCode::NONE;
  }

  void
  RemapUtil::fnToNormal(const RemapParams &params)
  {
    allFlagStatus.keypad = false;
    if (params.ex_origKey == KeyCode::KEYPAD_0) {
      *(params.key) = KeyCode::M;
    } else if (params.ex_origKey == KeyCode::KEYPAD_1) {
      *(params.key) = KeyCode::J;
    } else if (params.ex_origKey == KeyCode::KEYPAD_2) {
      *(params.key) = KeyCode::K;
    } else if (params.ex_origKey == KeyCode::KEYPAD_3) {
      *(params.key) = KeyCode::L;
    } else if (params.ex_origKey == KeyCode::KEYPAD_4) {
      *(params.key) = KeyCode::U;
    } else if (params.ex_origKey == KeyCode::KEYPAD_5) {
      *(params.key) = KeyCode::I;
    } else if (params.ex_origKey == KeyCode::KEYPAD_6) {
      *(params.key) = KeyCode::O;
    } else if (params.ex_origKey == KeyCode::KEYPAD_7) {
      *(params.key) = KeyCode::KEY_7;
    } else if (params.ex_origKey == KeyCode::KEYPAD_8) {
      *(params.key) = KeyCode::KEY_8;
    } else if (params.ex_origKey == KeyCode::KEYPAD_9) {
      *(params.key) = KeyCode::KEY_9;
    } else if (params.ex_origKey == KeyCode::KEYPAD_CLEAR) {
      *(params.key) = KeyCode::KEY_6;
    } else if (params.ex_origKey == KeyCode::KEYPAD_PLUS) {
      *(params.key) = KeyCode::SLASH;
    } else if (params.ex_origKey == KeyCode::KEYPAD_MINUS) {
      *(params.key) = KeyCode::SEMICOLON;
    } else if (params.ex_origKey == KeyCode::KEYPAD_MULTIPLY) {
      *(params.key) = KeyCode::P;
    } else if (params.ex_origKey == KeyCode::KEYPAD_SLASH) {
      *(params.key) = KeyCode::KEY_0;
    } else if (params.ex_origKey == KeyCode::KEYPAD_EQUAL) {
      *(params.key) = KeyCode::MINUS;
    } else if (params.ex_origKey == KeyCode::KEYPAD_DOT) {
      *(params.key) = KeyCode::DOT;
    } else if (params.ex_origKey == KeyCode::PAGEUP) {
      *(params.key) = KeyCode::CURSOR_UP;
      allFlagStatus.cursor = true;
    } else if (params.ex_origKey == KeyCode::PAGEDOWN) {
      *(params.key) = KeyCode::CURSOR_DOWN;
      allFlagStatus.cursor = true;
    } else if (params.ex_origKey == KeyCode::HOME) {
      *(params.key) = KeyCode::CURSOR_LEFT;
      allFlagStatus.cursor = true;
    } else if (params.ex_origKey == KeyCode::END) {
      *(params.key) = KeyCode::CURSOR_RIGHT;
      allFlagStatus.cursor = true;
    } else if (params.ex_origKey == getEnterKeyCode(params)) {
      *(params.key) = KeyCode::RETURN;
    } else if (params.ex_origKey == KeyCode::FORWARD_DELETE) {
      *(params.key) = KeyCode::DELETE;
    } else {
      return;
    }
  }

  void
  RemapUtil::toFN(const RemapParams &params) {
    if (! allFlagStatus.fn.isHeldDown()) return;

    // change cursor & return key behavior
    if (*(params.key) == KeyCode::CURSOR_UP) {
      *(params.key) = KeyCode::PAGEUP;
    } else if (*(params.key) == KeyCode::CURSOR_DOWN) {
      *(params.key) = KeyCode::PAGEDOWN;
    } else if (*(params.key) == KeyCode::CURSOR_LEFT) {
      *(params.key) = KeyCode::HOME;
    } else if (*(params.key) == KeyCode::CURSOR_RIGHT) {
      *(params.key) = KeyCode::END;
    } else if (*(params.key) == KeyCode::RETURN) {
      *(params.key) = getEnterKeyCode(params);
    } else if (*(params.key) == KeyCode::DELETE) {
      *(params.key) = KeyCode::FORWARD_DELETE;
    } else {
      return;
    }
    // strip CURSOR flag
    allFlagStatus.cursor = false;
  }

  KeyCode::KeyCode
  RemapUtil::getEnterKeyCode(const RemapParams &params)
  {
    if (*(params.keyboardType) == KeyboardType::POWERBOOK) {
      return KeyCode::ENTER_POWERBOOK;
    }
    return KeyCode::ENTER;
  }

  void
  RemapUtil::modifierToModifier(const RemapParams &params, ModifierFlag::ModifierFlag fromFlag, ModifierFlag::ModifierFlag toFlag)
  {
    KeyCode::KeyCode fromKeyCode = getModifierKeyCode(fromFlag);
    if (params.ex_origKey != fromKeyCode) return;

    FlagStatus *fromStatus = allFlagStatus.getFlagStatus(fromFlag);
    if (fromStatus == NULL) return;
    FlagStatus *toStatus = allFlagStatus.getFlagStatus(toFlag);
    if (toStatus == NULL) return;

    if (isModifierOn(params, fromFlag)) {
      toStatus->increase();
      fromStatus->decrease();
    } else {
      toStatus->decrease();
      fromStatus->increase();
    }

    KeyCode::KeyCode toKeyCode = getModifierKeyCode(toFlag);
    *(params.key) = toKeyCode;
  }

  void
  RemapUtil::modifierToKey(const RemapParams &params, ModifierFlag::ModifierFlag fromFlag, KeyCode::KeyCode toKeyCode) {
    KeyCode::KeyCode fromKeyCode = getModifierKeyCode(fromFlag);
    if (params.ex_origKey != fromKeyCode) return;

    FlagStatus *fromStatus = allFlagStatus.getFlagStatus(fromFlag);
    if (fromStatus == NULL) return;

    if (isModifierOn(params, fromFlag)) {
      fromStatus->decrease();
      *(params.eventType) = KeyEvent::DOWN;
    } else {
      fromStatus->increase();
      *(params.eventType) = KeyEvent::UP;
    }

    *(params.key) = toKeyCode;
  }

  void
  RemapUtil::keyToModifier(const RemapParams &params, KeyCode::KeyCode fromKeyCode, ModifierFlag::ModifierFlag toFlag) {
    if (params.ex_origKey != fromKeyCode) return;

    FlagStatus *toStatus = allFlagStatus.getFlagStatus(toFlag);
    if (toStatus == NULL) return;

    if (*(params.eventType) == KeyEvent::DOWN) {
      toStatus->increase();
    } else if (*(params.eventType) == KeyEvent::UP) {
      toStatus->decrease();
    }

    KeyCode::KeyCode toKeyCode = getModifierKeyCode(toFlag);
    *(params.key) = toKeyCode;
    *(params.eventType) = KeyEvent::MODIFY;
  }

  void
  RemapUtil::keyToModifier(const RemapParams &params, KeyCode::KeyCode fromKeyCode, ModifierFlag::ModifierFlag toFlag1, ModifierFlag::ModifierFlag toFlag2) {
    if (params.ex_origKey != fromKeyCode) return;

    FlagStatus *toStatus1 = allFlagStatus.getFlagStatus(toFlag1);
    FlagStatus *toStatus2 = allFlagStatus.getFlagStatus(toFlag2);
    if (toStatus1 == NULL || toStatus2 == NULL) return;

    if (*(params.eventType) == KeyEvent::DOWN) {
      toStatus1->increase();
      toStatus2->increase();
    } else if (*(params.eventType) == KeyEvent::UP) {
      toStatus1->decrease();
      toStatus2->decrease();
    }

    KeyCode::KeyCode toKeyCode = getModifierKeyCode(toFlag1);
    *(params.key) = toKeyCode;
    *(params.eventType) = KeyEvent::MODIFY;
  }

  void
  RemapUtil::keyToKey(const RemapParams &params, KeyCode::KeyCode fromKeyCode, KeyCode::KeyCode toKeyCode) {
    if (params.ex_origKey != fromKeyCode) return;
    *(params.key) = toKeyCode;
  }

  // --------------------
  void
  RemapUtil::jis_toggle_eisuu_kana(const RemapParams &params, KeyCode::KeyCode fromKeyCode) {
    if (params.ex_origKey != fromKeyCode) return;

    static bool isKana = true;

    if (isKana) {
      RemapUtil::keyToKey(params, fromKeyCode, KeyCode::JIS_KANA);
    } else {
      RemapUtil::keyToKey(params, fromKeyCode, KeyCode::JIS_EISUU);
    }
    if (*(params.eventType) == KeyEvent::UP) {
      isKana = ! isKana;
    }
  }

  void
  RemapUtil::jis_toggle_eisuu_kana(const RemapParams &params, ModifierFlag::ModifierFlag fromModifier) {
    KeyCode::KeyCode fromKeyCode = getModifierKeyCode(fromModifier);

    if (params.ex_origKey != fromKeyCode) return;

    static bool isKana = true;

    if (isKana) {
      RemapUtil::modifierToKey(params, fromModifier, KeyCode::JIS_KANA);
    } else {
      RemapUtil::modifierToKey(params, fromModifier, KeyCode::JIS_EISUU);
    }
    if (*(params.eventType) == KeyEvent::UP) {
      isKana = ! isKana;
    }
  }

  // ----------------------------------------------------------------------
  void
  FlagStatus::initialize(const RemapParams &params)
  {
    temporary_count = 0;

    if (*(params.eventType) != KeyEvent::MODIFY) return;
    if (params.ex_origKey != key) return;

    if (RemapUtil::isModifierOn(params, flag)) {
      increase();
    } else {
      decrease();
    }
  }

  void
  AllFlagStatus::initialize(const RemapParams &params)
  {
    if (*(params.eventType) == KeyEvent::DOWN) ++numHeldDownKeys;
    if (*(params.eventType) == KeyEvent::UP) --numHeldDownKeys;

#if 0
    if (config.debug) {
      printf("KeyRemap4MacBook numHeldDownKeys = %d\n", numHeldDownKeys);
    }
#endif

    capslock.initialize(params);
    shiftL.initialize(params);
    shiftR.initialize(params);
    controlL.initialize(params);
    controlR.initialize(params);
    optionL.initialize(params);
    optionR.initialize(params);
    commandL.initialize(params);
    commandR.initialize(params);
    fn.initialize(params);

    // *** Note *** ModifierFlag::CURSOR == ModifierFlag::KEYPAD
    if (RemapUtil::isModifierOn(params, ModifierFlag::CURSOR)) {
      if (params.ex_origKey == KeyCode::CURSOR_UP ||
          params.ex_origKey == KeyCode::CURSOR_DOWN ||
          params.ex_origKey == KeyCode::CURSOR_LEFT ||
          params.ex_origKey == KeyCode::CURSOR_RIGHT) {
        cursor = true;
      } else {
        keypad = true;
      }
    } else {
      cursor = keypad = false;
    }
  }

  void
  AllFlagStatus::reset(void)
  {
#if 0
    if (config.debug) {
      printf("KeyRemap4MacBook::AllFlagStatus::reset\n");
    }
#endif

    capslock.reset();
    shiftL.reset();
    shiftR.reset();
    controlL.reset();
    controlR.reset();
    optionL.reset();
    optionR.reset();
    commandL.reset();
    commandR.reset();
    fn.reset();
    cursor = false;
    keypad = false;
    numHeldDownKeys = 0;
  }

  unsigned int
  AllFlagStatus::makeFlags(const RemapParams &params)
  {
    if (numHeldDownKeys <= 0 && *(params.flags) == 0) {
      reset();
    }

    unsigned int flags = 0;
    flags |= capslock.makeFlag();
    flags |= shiftL.makeFlag();
    flags |= shiftR.makeFlag();
    flags |= controlL.makeFlag();
    flags |= controlR.makeFlag();
    flags |= optionL.makeFlag();
    flags |= optionR.makeFlag();
    flags |= commandL.makeFlag();
    flags |= commandR.makeFlag();
    flags |= fn.makeFlag();

    if (cursor) {
      flags |= ModifierFlag::CURSOR;
    }
    if (keypad) {
      flags |= ModifierFlag::KEYPAD;
    }
    return flags;
  }

  FlagStatus *
  AllFlagStatus::getFlagStatus(ModifierFlag::ModifierFlag flag) {
    if (flag == ModifierFlag::CAPSLOCK) return &capslock;
    if (flag == ModifierFlag::SHIFT_L) return &shiftL;
    if (flag == ModifierFlag::SHIFT_R) return &shiftR;
    if (flag == ModifierFlag::CONTROL_L) return &controlL;
    if (flag == ModifierFlag::CONTROL_R) return &controlR;
    if (flag == ModifierFlag::OPTION_L) return &optionL;
    if (flag == ModifierFlag::OPTION_R) return &optionR;
    if (flag == ModifierFlag::COMMAND_L) return &commandL;
    if (flag == ModifierFlag::COMMAND_R) return &commandR;
    if (flag == ModifierFlag::FN) return &fn;
    return NULL;
  }

  // ----------------------------------------------------------------------
  void
  ListFireExtraKey::reset(void)
  {
    for (int i = 0; i < FIREEXTRAKEY_MAXNUM; ++i) {
      list[i].unset();
    }
  }

  void
  ListFireExtraKey::add(FireExtraKey::Type type, unsigned int eventType, unsigned int flags, unsigned int key, unsigned int charCode)
  {
    for (int i = 0; i < FIREEXTRAKEY_MAXNUM; ++i) {
      if (! list[i].isEnable()) {
        list[i].set(type, eventType, flags, key, charCode);
        break;
      }
    }
  }

  void
  ListFireExtraKey::fire(FireExtraKey::Type type, KeyboardEventAction action, OSObject *target, const RemapParams &params)
  {
    for (int i = 0; i < FIREEXTRAKEY_MAXNUM; ++i) {
      FireExtraKey &item = list[i];

      if (item.isEnable() && item.getType() == type) {
        action(target, item.getEventType(), item.getFlags(), item.getKey(), item.getCharCode(),
               *(params.charSet), *(params.origCharCode), *(params.origCharSet),
               *(params.keyboardType), false, *(params.ts));
      }
    }
  }

  // ----------------------------------------------------------------------
  void
  ModifierCanceling::keyRelease(const RemapParams &params)
  {
    if (flagStatus == NULL) return;

    flagStatus->temporary_decrease();

    if (isSendKeyRelease) return;
    unsigned int flags = allFlagStatus.makeFlags(params);

    listFireExtraKey.add(FireExtraKey::TYPE_BEFORE, KeyEvent::MODIFY, flags, keycode, 0);
    isSendKeyRelease = true;
  }

  void
  ModifierCanceling::restore(const RemapParams &params)
  {
    if (flagStatus == NULL) return;

    if (! isSendKeyRelease) return;

    // already released, ignore isSendKeyRelease;
    if (! flagStatus->isHeldDown()) {
      isSendKeyRelease = false;
      return;
    }

    if (*(params.eventType) == KeyEvent::DOWN) {
      isSendKeyRelease = false;
      unsigned int flags = allFlagStatus.makeFlags(params);
      listFireExtraKey.add(FireExtraKey::TYPE_BEFORE, KeyEvent::MODIFY, flags, keycode, 0);
    }
  }
}
