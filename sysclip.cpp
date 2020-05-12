#include "sysclip.hpp"

#include <gtk-3.0/gtk/gtk.h>

#include <thread>

static bool sysclip_started = false;
static std::thread sysclip_main;
static std::string clip_content;

bool copy_to_sysclip(const std::string& content)
{
	if(!sysclip_started) {
		gtk_init(nullptr, nullptr);
		sysclip_main = std::thread(gtk_main);
		sysclip_main.detach();
		sysclip_started = true;
	}

	GSourceFunc fn = [] (gpointer) -> gboolean {
		auto clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		gtk_clipboard_set_text(clipboard, clip_content.c_str(), clip_content.size());
		return G_SOURCE_REMOVE;
	};

	// copy it over
	clip_content = content;

	// gtk doesn't like multithreading
	gdk_threads_add_idle(fn, nullptr);

	return true; // we can't tell if it succeeded or not
}

void deinit_sysclip()
{
	if(sysclip_started) {
		gtk_main_quit();
	}
}
