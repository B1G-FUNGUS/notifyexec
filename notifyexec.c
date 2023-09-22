#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include <glib.h>

#include <stdlib.h>
#include <libpurple/notify.h>
#include <libpurple/plugin.h>
#include <libpurple/version.h>
#include <libpurple/prefs.h>

PurplePlugin *notify_exec_plugin = NULL;

static PurplePluginPrefFrame *
get_plugin_pref_frame(PurplePlugin *plugin) {
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;

	frame = purple_plugin_pref_frame_new();

	ppref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/notify_exec/command", "Command");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_label(
		"Can use the following environment variables\n"
		"'$ne_sender' - the message sender\n"
		"'$ne_message' - the message\n"
		"'$ne_conversation_name' - the conversation name\n"
		"'$ne_conversation_title' - the conversation alias\n"
		"'$ne_account_name' - the account name\n"
		"'$ne_account_alias' - the account alias\n");
	purple_plugin_pref_frame_add(frame, ppref);

	return frame;
}

static PurplePluginUiInfo prefs_info = {
	get_plugin_pref_frame,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void 
notify_exec_command(PurpleAccount *account, char *sender, char *message,
	PurpleConversation *conv, PurpleMessageFlags flags) 
{
	gchar **envp;
	gchar **argv;

	const gchar *command =
		purple_prefs_get_string("/plugins/core/notify_exec/command");
	
	if (!strcmp(command, "")) return;

	g_shell_parse_argv(command, NULL, &argv, NULL);

	envp = g_get_environ();
	envp = g_environ_setenv(envp, "ne_sender", sender, TRUE);
	envp = g_environ_setenv(envp, "ne_message", message, TRUE);
	envp = g_environ_setenv(envp, "ne_conversation_name", conv->name, TRUE);
	envp = g_environ_setenv(envp, "ne_conversation_title", conv->title, TRUE);
	envp = g_environ_setenv(envp, "ne_account_name", account->username, TRUE);
	envp = g_environ_setenv(envp, "ne_account_alias", account->alias, TRUE);

	GError *gerror = NULL;

	if (!g_spawn_sync(NULL, argv, envp, G_SPAWN_SEARCH_PATH_FROM_ENVP,
		NULL, NULL, NULL, NULL, NULL, &gerror))
		purple_notify_error(notify_exec_plugin, "Notify Exec", 
		"Failed to execute command", gerror->message);

	g_strfreev(envp);
	g_strfreev(argv);
}

static void
notify_exec_recieved_im_msg(PurpleAccount *account, char *sender, char
	*message, PurpleConversation *conv, PurpleMessageFlags flags)
{
	if (!conv->ui_ops->has_focus(conv))
		notify_exec_command(account, sender, message, conv, flags);
}

static void
notify_exec_recieved_chat_msg(PurpleAccount *account, char *sender, char
	*message, PurpleConversation *conv, PurpleMessageFlags flags)
{
	if (!conv->ui_ops->has_focus(conv)) 
		notify_exec_command(account, sender, message, conv, flags);
}

static gboolean
plugin_load(PurplePlugin *plugin) 
{
	notify_exec_plugin = plugin;

	purple_signal_connect(purple_conversations_get_handle(),
		"received-im-msg", plugin,
		PURPLE_CALLBACK(notify_exec_recieved_im_msg), NULL);
		
	purple_signal_connect(purple_conversations_get_handle(),
		"received-chat-msg", plugin,
		PURPLE_CALLBACK(notify_exec_recieved_chat_msg), NULL);

	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin) {
	purple_signal_disconnect(purple_conversations_get_handle(),
		"received-im-msg", plugin,
		PURPLE_CALLBACK(notify_exec_recieved_im_msg));
		
	purple_signal_disconnect(purple_conversations_get_handle(),
		"received-chat-msg", plugin,
		PURPLE_CALLBACK(notify_exec_recieved_chat_msg));

	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	"core-notify_exec",
	"Notify Exec",
	"1.1",

	"Custom Notification Command",          
	"Execute a custom command to notify you of recieved messages",          
	"B1G_FUNGUS <TODO>",                          
	"<TODO>",     

	plugin_load,                   
	plugin_unload,                          
	NULL,                          

	NULL,                          
	NULL,                          
	&prefs_info,                        
	NULL,                   

	NULL,                          
	NULL,                          
	NULL,                          
	NULL                           
};                               

static void                        
init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none("/plugins/core/notify_exec");
	purple_prefs_add_string("/plugins/core/notify_exec/command", "");
}

PURPLE_INIT_PLUGIN(hello_world, init_plugin, info)
