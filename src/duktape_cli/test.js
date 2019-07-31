

function fa () {
	fd();
}

function fb () {
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
	}
	wnd.OnClose = function () {
		PostQuitMessage();
	}
	wnd.Create(null, 0, 0, 800, 600);
	wnd.SetVisible(true);

	edit.OnChange = function() {
		print("edit changed: " + edit.GetText());
	}
	
	RunMessageLoop();
	edit.close();
	btn1.close();
	wnd.close();
	font.close();
	fc();
}

fb();

