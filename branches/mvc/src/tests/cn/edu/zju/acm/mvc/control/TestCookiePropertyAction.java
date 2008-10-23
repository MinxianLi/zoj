
package cn.edu.zju.acm.mvc.control;

import cn.edu.zju.acm.mvc.control.annotation.Cookie;

public class TestCookiePropertyAction extends TestActionBase {

    private String stringProp;
    
    private Object objProp;

    @Cookie
    public String getStringProp() {
        return this.stringProp;
    }

    @Cookie
    public void setStringProp(String stringProp) {
        this.stringProp = stringProp;
    }

    @Cookie
    public Object getObjProp() {
        return this.objProp;
    }

    @Cookie
    public void setObjProp(Object objProp) {
        this.objProp = objProp;
    }

}
