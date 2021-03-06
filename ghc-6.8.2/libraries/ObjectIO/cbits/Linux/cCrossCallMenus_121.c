/********************************************************************************************
	Clean OS Windows library module version 1.2.1.
	This module is part of the Clean Object I/O library, version 1.2.1,
	for the Windows platform.
********************************************************************************************/

/********************************************************************************************
	About this module:
	Routines related to menu handling.
********************************************************************************************/
#include "cCrossCallMenus_121.h"
#include "cCrossCall_121.h"


/*	Cross call procedure implementations.
	Eval<nr> corresponds with a CrossCallEntry generated by NewCrossCallEntry (nr,Eval<nr>).
*/
/*	Remove a shortkey from a framewindow shortkey table. */

static gboolean dummy_find_accel(GtkAccelKey *key, GClosure *closure, gpointer data)
{
	return gtk_true();
}

void EvalCcRqREMOVEMENUSHORTKEY (CrossCallInfo *pcci)	/* frameptr, cmd; no result. */
{
	GtkWidget *frame;
	GtkWidget *box;
	GtkWidget *menu_item;
	GtkAccelGroup *accel_group;

	frame = (GtkWidget *) pcci->p1;
	menu_item = (GtkWidget *) pcci->p2;

	accel_group = ((GtkAccelGroup *) gtk_accel_groups_from_object (G_OBJECT(frame))->data);

	for (;;)
	{
		GtkAccelKey *key = gtk_accel_group_find(accel_group, dummy_find_accel, NULL);
		if (!key) break;

		gtk_widget_remove_accelerator(menu_item,
									  accel_group,
									  key->accel_key,
									  key->accel_mods);
	}

	MakeReturn0Cci (pcci);
}

void EvalCcRqMODIFYMENUITEM (CrossCallInfo *pcci)	/* hitem, hmenu, textptr; no result.	*/
{
	GtkWidget *menu, *menu_item, *label;
	gchar *title;

	title = createMnemonicString((gchar *) pcci->p3);

	menu = (GtkWidget *) pcci->p2;
	menu_item = (GtkWidget *) pcci->p1;
	label = gtk_bin_get_child(GTK_BIN(menu_item));
	gtk_label_set_text_with_mnemonic(GTK_LABEL(label), title);

	rfree(title);

	MakeReturn0Cci (pcci);
}

static int in_handler_flag = 0;

static void menuitem_activate_handler(GtkMenuItem *menu_item)
{
	if (in_handler_flag == 0)
	{
		in_handler_flag = 1;
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), !(GTK_CHECK_MENU_ITEM(menu_item)->active));
		SendMessage2ToClean (CcWmCOMMAND, GTK_WIDGET(menu_item), GetModifiers ());
		in_handler_flag = 0;
	}
}

void EvalCcRqINSERTMENUITEM (CrossCallInfo *pcci)
{
	gchar *title;
	GtkWidget *frame, *menu, *menu_item, *label;
	GtkAccelGroup *accel_group;
	char key;

	title = createMnemonicString((gchar *) pcci->p4);

	frame = (GtkWidget *) pcci->p2;
	menu = (GtkWidget *) pcci->p3;
	key = (char) pcci->p5;

	menu_item = gtk_check_menu_item_new();
	label = gtk_accel_label_new("");
	gtk_label_set_text_with_mnemonic(GTK_LABEL(label), title);
	gtk_container_add(GTK_CONTAINER(menu_item), label);
	gtk_accel_label_set_accel_widget(GTK_ACCEL_LABEL(label), menu_item);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), ((pcci->p1 & 1) != 0));
	gtk_widget_set_sensitive(menu_item, ((pcci->p1 & 2) != 0));

	gtk_signal_connect_object (GTK_OBJECT (menu_item), "activate",
		GTK_SIGNAL_FUNC (menuitem_activate_handler), menu_item);

	gtk_menu_insert(GTK_MENU(menu), menu_item, (gint) pcci->p6);
	gtk_widget_show_all(menu_item);

	rfree(title);


	if (key != 0)
	{
		accel_group = ((GtkAccelGroup *) gtk_accel_groups_from_object (G_OBJECT(frame))->data);

		gtk_widget_add_accelerator(menu_item, "activate",
								   accel_group,
								   key,
								   GDK_CONTROL_MASK,
								   GTK_ACCEL_VISIBLE);
		gtk_widget_add_accelerator(menu_item, "activate",
								   accel_group,
								   key,
								   GDK_CONTROL_MASK | GDK_SHIFT_MASK,
								   0);
		gtk_widget_add_accelerator(menu_item, "activate",
								   accel_group,
								   key,
								   GDK_CONTROL_MASK | GDK_MOD1_MASK,
								   0);
		gtk_widget_add_accelerator(menu_item, "activate",
								   accel_group,
								   key,
								   GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK,
								   0);
	}

	MakeReturn1Cci (pcci, (int) menu_item);
}

void EvalCcRqITEMENABLE (CrossCallInfo *pcci)	/* parent, HITEM, onoff; no result.  */
{
	GtkWidget *menu, *menu_item;
	menu      = (GtkWidget *) pcci->p1;
	menu_item = (GtkWidget *) pcci->p2;

	gtk_widget_set_sensitive(menu_item, (gboolean) pcci->p3);

	MakeReturn0Cci (pcci);
}

static void find_item_callback(GtkWidget *menu_item, gpointer data)
{
	if (GTK_IS_MENU_ITEM(menu_item) && GTK_MENU_ITEM (menu_item)->submenu == ((GtkWidget *) data))
		*((GtkWidget **) data) = menu_item;
};

/*	Remove a menu logically */
void EvalCcRqDELETEMENU (CrossCallInfo *pcci)			/* HMENU, HITEM; no result. */
{
	GtkWidget *menu, *menu_item;
	menu = (GtkWidget *) pcci->p1;
	menu_item = (GtkWidget *) pcci->p2;

	gtk_container_foreach(GTK_CONTAINER(menu), find_item_callback, (gpointer) &menu_item);
	if (menu_item != (GtkWidget *) pcci->p2)
	{
		gtk_menu_item_remove_submenu(GTK_MENU_ITEM(menu_item));
		gtk_widget_destroy(menu_item);
	}

	MakeReturn0Cci (pcci);
}

void EvalCcRqREMOVEMENUITEM (CrossCallInfo *pcci)		/* menu, HITEM; no result. */
{
	GtkWidget *menu, *menu_item;
	menu = (GtkWidget *) pcci->p1;
	menu_item = (GtkWidget *) pcci->p2;

	gtk_menu_item_remove_submenu(GTK_MENU_ITEM(menu_item));
	gtk_widget_destroy(menu_item);

	MakeReturn0Cci (pcci);
}

void EvalCcRqINSERTSEPARATOR (CrossCallInfo *pcci)		/* hmenu, pos no result. */
{
	GtkWidget *menu, *menu_item;

	menu = (GtkWidget *) pcci->p1;

	menu_item = gtk_menu_item_new();
	gtk_menu_insert(GTK_MENU(menu), menu_item, (gint) pcci->p2);
	gtk_widget_show_all(menu_item);

	MakeReturn1Cci (pcci, (int) menu_item);
}

void EvalCcRqMODIFYMENU (CrossCallInfo *pcci)	/* hitem, hmenu, textptr; no result.	*/
{
	gint i;
	GtkWidget *menu, *menu_item, *label;
	gchar *title;

	title = createMnemonicString((gchar *) pcci->p3);

	menu = (GtkWidget *) pcci->p2;
	menu_item = (GtkWidget *) pcci->p1;
	label = gtk_bin_get_child(GTK_BIN(menu_item));
	gtk_label_set_text_with_mnemonic(GTK_LABEL(label), title);

	rfree(title);

	MakeReturn0Cci (pcci);
}

/*	Insert a menu into the menu bar. */
void EvalCcRqINSERTMENU (CrossCallInfo *pcci)
{
	int i;
	gchar *title;
	GtkWidget *parent_menu, *root_menu, *sub_menu;
	GtkAccelGroup *accel_group;

	title = createMnemonicString((gchar *) pcci->p3);
	parent_menu = (GtkWidget *) pcci->p2;
	sub_menu = gtk_menu_new();

	if (GTK_IS_MENU_BAR(parent_menu))
	{
		GtkWidget *frame;
		frame = gtk_widget_get_parent(gtk_widget_get_parent(parent_menu));
		accel_group = ((GtkAccelGroup *) gtk_accel_groups_from_object (G_OBJECT(frame))->data);
	}
	else
	{
		accel_group = gtk_menu_get_accel_group (GTK_MENU(parent_menu));
	}

	gtk_menu_set_accel_group (GTK_MENU(sub_menu), accel_group);

	root_menu = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(root_menu), gtk_label_new_with_mnemonic(title));
	gtk_widget_set_sensitive(root_menu, (gboolean) pcci->p1);
    gtk_widget_show_all (root_menu);

    gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), sub_menu);

    if (GTK_IS_MENU_BAR(parent_menu))
    	gtk_menu_bar_insert(GTK_MENU_BAR(parent_menu), root_menu, (gint) pcci->p5);
	else
		gtk_menu_insert(GTK_MENU(parent_menu), root_menu, (gint) pcci->p5);

	rfree(title);

	MakeReturn1Cci (pcci, (int) sub_menu);
}

static void enable_menu_callback(GtkWidget *menu_item, gpointer data)
{
	if (GTK_IS_MENU_ITEM(menu_item) && GTK_MENU_ITEM (menu_item)->submenu == ((GtkWidget *) data))
		gtk_widget_set_sensitive(menu_item, gtk_true());
};

static void disable_menu_callback(GtkWidget *menu_item, gpointer data)
{
	if (GTK_IS_MENU_ITEM(menu_item) && GTK_MENU_ITEM (menu_item)->submenu == ((GtkWidget *) data))
		gtk_widget_set_sensitive(menu_item, gtk_false());
};

void EvalCcRqMENUENABLE (CrossCallInfo *pcci)	/* parent, zero based position of menu, onoff; no result. */
{
	GtkWidget *parent_menu, *sub_menu;
	parent_menu = (GtkWidget *) pcci->p1;
	sub_menu    = (GtkWidget *) pcci->p2;

	gtk_container_foreach(GTK_CONTAINER(parent_menu),
						  pcci->p3 ? enable_menu_callback : disable_menu_callback,
						  (gpointer) sub_menu);

	MakeReturn0Cci (pcci);
}

void EvalCcRqDRAWMBAR (CrossCallInfo *pcci)		/* framePtr, clientPtr; no result. */
{
	MakeReturn0Cci (pcci);
}

/*	Track pop up menu. */
void EvalCcRqTRACKPOPMENU (CrossCallInfo *pcci)	/* popupmenu,framePtr; BOOL result. */
{
	GtkWidget *popup_menu;
	GtkWidget *frame;
	GdkEvent  *event;

	popup_menu = (GtkWidget *) pcci->p1;
	frame      = (GtkWidget *) pcci->p2;

	event = gtk_get_current_event();
	gtk_menu_popup(GTK_MENU(popup_menu),NULL,NULL,NULL,NULL,
			(event->type == GDK_BUTTON_PRESS) ? ((GdkEventButton *) event)->button : 0,
			gdk_event_get_time(event));

	MakeReturn1Cci (pcci,(int)gtk_true());
}

void EvalCcRqCREATEPOPMENU (CrossCallInfo *pcci) /* no params; MENU result.   */
{
	MakeReturn1Cci (pcci, (int) gtk_menu_new());
}

void EvalCcRqCHECKMENUITEM (CrossCallInfo *pcci) /* menu, HITEM, on/off; no result.	*/
{
	GtkWidget *menu, *menu_item;
	menu      = (GtkWidget *) pcci->p1;
	menu_item = (GtkWidget *) pcci->p2;

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item), (gboolean) pcci->p3);

	MakeReturn0Cci (pcci);
}


/*	Install the cross call procedures in the gCrossCallProcedureTable of cCrossCall_121.
*/
void InstallCrossCallMenus ()
{
	CrossCallProcedureTable newTable;

	newTable = EmptyCrossCallProcedureTable ();
	AddCrossCallEntry (newTable, CcRqREMOVEMENUSHORTKEY, EvalCcRqREMOVEMENUSHORTKEY);
	AddCrossCallEntry (newTable, CcRqMODIFYMENUITEM,     EvalCcRqMODIFYMENUITEM);
	AddCrossCallEntry (newTable, CcRqINSERTMENUITEM,     EvalCcRqINSERTMENUITEM);
	AddCrossCallEntry (newTable, CcRqITEMENABLE,         EvalCcRqITEMENABLE);
	AddCrossCallEntry (newTable, CcRqDELETEMENU,         EvalCcRqDELETEMENU);
	AddCrossCallEntry (newTable, CcRqREMOVEMENUITEM,     EvalCcRqREMOVEMENUITEM);
	AddCrossCallEntry (newTable, CcRqINSERTSEPARATOR,    EvalCcRqINSERTSEPARATOR);
	AddCrossCallEntry (newTable, CcRqMODIFYMENU,         EvalCcRqMODIFYMENU);
	AddCrossCallEntry (newTable, CcRqINSERTMENU,         EvalCcRqINSERTMENU);
	AddCrossCallEntry (newTable, CcRqMENUENABLE,         EvalCcRqMENUENABLE);
	AddCrossCallEntry (newTable, CcRqDRAWMBAR,           EvalCcRqDRAWMBAR);
	AddCrossCallEntry (newTable, CcRqTRACKPOPMENU,       EvalCcRqTRACKPOPMENU);
	AddCrossCallEntry (newTable, CcRqCREATEPOPMENU,      EvalCcRqCREATEPOPMENU);
	AddCrossCallEntry (newTable, CcRqCHECKMENUITEM,      EvalCcRqCHECKMENUITEM);
	AddCrossCallEntries (gCrossCallProcedureTable, newTable);
}
