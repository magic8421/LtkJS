

function fa () {
	fd();
}

function fb () {
	/*
	var win32 = LtkConstantTable();
	print("win32", win32);
	for (var name in win32) {
		print(name, win32[name]);
	}
	*/
	fa();
}

function fc () {
	print(traceback());
	
	var ts = new TestClass(255);
	ts.Print();
	ts.name = "ni hao";
	var rc = new Rect(10, 10, 800, 600);
	ts.SetRect(rc);
	rc.close();
	print(ts.name);
	ts.close();
}

function fd () {
	RegisterWndClass();
	var wnd = new LtkWindow();
	var edit = new LtkEdit();
	var font = new LtkFont();
	var btn1 = new LtkButton();
	var listview = new LtkListView();

	wnd.OnCreate = function () {
		print("OnCreate callback");
		wnd.SetText("你好,世界!");
		edit.Create(wnd, 10, 10, 400, 35);
		edit.SetVisible(true);
		font.CreateSimple("微软雅黑", 24);
		edit.SetFont(font, true);
		btn1.Create(wnd, 400 + 10 + 5, 10, 80, 35);
		btn1.SetText("提交");
		btn1.SetFont(font, true); // TODO 怎样一次设置整个窗口的字体?
		listview.Create(wnd, 10, 50, 500, 500);
		listview.InsertColumn(0, "001", 100, 0);
		listview.InsertColumn(1, "002", 140, 0);
		listview.InsertItem(0, "fffff");
		listview.InsertItem(1, "bbbbbbb");
	}
	wnd.OnClose = function () {
		PostQuitMessage();
	}
	wnd.Create(null, 0, 0, 800, 600);
	wnd.SetVisible(true);

	edit.OnChange = function() {
		print("edit changed: ", edit.GetText());
	}
	btn1.OnClick = function () {
		print("btn1 OnClick");
	}
	
	RunMessageLoop();
	edit.close();
	btn1.close();
	listview.close();
	wnd.close();
	font.close();
	fc();
}

fb();

