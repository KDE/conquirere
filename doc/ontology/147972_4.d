format 74

classcanvas 128004 class_ref 143620 // pimo:Event
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyzwh 120 47 2000 121 115
end
classcanvas 128132 class_ref 128004 // nbib:Publication
  draw_all_relations default hide_attributes yes hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyzwh 440 45 3005 129 117
end
classcanvas 128644 class_ref 198532 // ncal:Event
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyzwh 103 254 2000 157 127
end
note 128900 "changed by akonadi feeder"
  xyzwh 331 282 2000 131 65
relationcanvas 128260 relation_ref 219140 // <aggregation>
  decenter_begin 256
  decenter_end 243
  from ref 128132 z 3006 to ref 128004
  role_a_pos 250 48 3000 no_role_b
  multiplicity_a_pos 250 84 3000 no_multiplicity_b
end
relationcanvas 128388 relation_ref 219012 // <aggregation>
  decenter_begin 669
  decenter_end 675
  from ref 128004 z 3006 to ref 128132
  role_a_pos 291 97 3000 no_role_b
  multiplicity_a_pos 423 133 3000 no_multiplicity_b
end
relationcanvas 128772 relation_ref 245764 // <aggregation>
  from ref 128004 z 2001 to ref 128644
  role_a_pos 190 227 3000 no_role_b
  no_multiplicity_a no_multiplicity_b
end
line 129028 -_-_
  from ref 128900 z 2001 to ref 128644
end
