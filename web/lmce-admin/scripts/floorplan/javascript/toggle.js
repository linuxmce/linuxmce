function toggle(blnState, id_field)
{
	if (document.getElementById)
	{
		var field = document.getElementById(id_field);
		if (field)
		{
			field.style.display = (blnState == 0) ? 'none' : '';
			return true;
		}
	}
	return false;
}

function set_value(value, id_field)
{
	if (document.getElementById)
	{
		var field = document.getElementById(id_field);
		field.value = value;
		return true;
	}
	else return false;
}

function set_selection(set_index, id_select)
{
	if (document.getElementById)
	{
		var sel = document.getElementById(id_select);
		sel.selectedIndex = set_index;
	}
	else return false;
}

function is_selected(id_select, opt_value)
{
	if (document.getElementById)
	{
		var sel = document.getElementById(id_select);
		
		if (sel.options[sel.selectedIndex].value == opt_value) return true;
		else return false;
	}
	else return false;
}

function relate_selection(id_select, opt_value, id_related_tb)
{
	if (document.getElementById)
	{
		if (is_selected(id_select, opt_value))
		{
			toggle(1,id_related_tb);
		}
		else
		{
			toggle(0,id_related_tb);
			set_value('',id_related_tb);
		}
	}
	else return false;
}

function relate_selections(ids_select, opt_value, ids_related_tb)
{
	for (i=0; i < ids_select.length; i++)
	{
		relate_selection(ids_select[i], opt_value, ids_related_tb[i]);
	}
}

function set_selections(set_index, ids_select)
{
	if (document.getElementById)
	{
		for (i=0; i < ids_select.length; i++)
		{
			var sel = document.getElementById(ids_select[i]);
			sel.selectedIndex = set_index;
		}
	}
	else return false;
}

function hide_all(ids_field)
{
	if (document.getElementById)
	{
		for (i=0; i < ids_field.length; i++)
		{
			toggle(0,ids_field[i]);
		}
	}
	else return false;
}

function cb_relate(id_checkbox, id_related_item)
{
	if (document.getElementById)
	{
		var cb = document.getElementById(id_checkbox);
		if (cb.checked) return toggle(1,id_related_item);
		else return toggle(0,id_related_item);
	}
	else return false;
}

function cb_relate_all(ids_checkbox, ids_related_item)
{
	if (document.getElementById)
	{
		var success = false;
		for (i=0; i < ids_related_item.length; i++)
		{
			if (typeof(ids_checkbox)=="string") success = cb_relate(ids_checkbox, ids_related_item[i]);
			else success = cb_relate(ids_checkbox[i], ids_related_item[i]);
		}
		return success;
	}
	else return false;
}